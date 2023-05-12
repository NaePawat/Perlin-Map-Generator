// Fill out your copyright notice in the Description page of Project Settings.


#include "SortieCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/Character/SCharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MCChunk.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

ASortieCharacterBase::ASortieCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setting up components for the character
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	ACharacter::GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	//TODO: Add network replication functionality
}

void ASortieCharacterBase::MoveForward(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		const FRotator currentRotation = FRotator(0, GetControlRotation().Yaw, 0);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(currentRotation)*DirectionValue);
	}
}

void ASortieCharacterBase::MoveRight(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		const FRotator currentRotation = FRotator(0, GetControlRotation().Yaw, 0);
		AddMovementInput(UKismetMathLibrary::GetRightVector(currentRotation)*DirectionValue);
	}
}

void ASortieCharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if(GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASortieCharacterBase::Jump()
{
	Super::Jump();
}

void ASortieCharacterBase::Fire()
{
	EditTerrain(false, IsFiring);
}

void ASortieCharacterBase::StopFire()
{
	IsFiring = false;
}

void ASortieCharacterBase::Aim()
{
	EditTerrain(true, IsAiming);
}

void ASortieCharacterBase::StopAim()
{
	IsAiming = false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASortieCharacterBase::ChangeGravityDirection()
{
	if(const FHitResult HitResult = LineTraceFromCamera(); HitResult.bBlockingHit)
	{
		USCharacterMovementComponent* GravityMovement = GetGravityMovementComponent();
		GravityMovement->SetGravityDirection(CameraComp->GetComponentRotation().Vector());
	}
}

void ASortieCharacterBase::EditTerrain(const bool Add, bool ToggleAction)
{
	if(!ToggleAction)
	{
		TArray<AActor*> IgnoreActor;
		IgnoreActor.Init(this, 1);
			
		const FVector SphereSpawnPoint = LineTraceFromCamera().ImpactPoint;

		//Set what actor to seek out from it's collision channel
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		TArray<AActor*> OutActors;
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SphereSpawnPoint, SphereRadius, TraceObjectTypes, AMCChunk::StaticClass(), IgnoreActor, OutActors);
		DrawDebugSphere(GetWorld(), SphereSpawnPoint, SphereRadius, 16 , FColor::Red, false, 5.f, 0u, 0.f);
		for(AActor* Actor: OutActors)
		{
			if(AMCChunk* ImpactedMarchingCube = Cast<AMCChunk>(Actor))
			{
				ImpactedMarchingCube->Terraform(SphereSpawnPoint, SphereRadius, Add ? BrushForce : -BrushForce);
			}
		}

		ToggleAction = true;
	}
}

USCharacterMovementComponent* ASortieCharacterBase::GetGravityMovementComponent() const
{
	return Cast<USCharacterMovementComponent>(GetMovementComponent());
}

FHitResult ASortieCharacterBase::LineTraceFromCamera() const
{
	FHitResult HitResult;
			
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
			
	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = CameraComp->GetComponentLocation() + CameraComp->GetComponentRotation().Vector() * LineTraceDistance;

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, HitResult.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 1.0f);

	return HitResult;
}

// Called when the game starts or when spawned
void ASortieCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//setup input
	if(const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

// Called every frame
void ASortieCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASortieCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Setup Input Binding
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this,&ASortieCharacterBase::MoveForward);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::MoveRight);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ASortieCharacterBase::StopFire);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ASortieCharacterBase::StopAim);
		EnhancedInputComponent->BindAction(ChangeGravAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::ChangeGravityDirection);
	}

}

