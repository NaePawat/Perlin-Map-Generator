// Fill out your copyright notice in the Description page of Project Settings.


#include "SortieCharacterBase.h"
#include "AI/SortieAI.h"
#include "Camera/CameraComponent.h"
#include "Components/Character/SCharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Map/MCChunk.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

void ASortieCharacterBase::MoveForward(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		/*const FRotator currentRotation = FRotator(0, GetControlRotation().Yaw, 0);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(currentRotation)*DirectionValue);*/
		FVector ForwardDirection = UKismetMathLibrary::Cross_VectorVector(
		UKismetMathLibrary::GetRightVector(GetControlRotation()),
		GetCapsuleComponent()->GetUpVector()
		);

		ForwardDirection.Normalize();
		
		AddMovementInput(ForwardDirection*DirectionValue);
	}
}

void ASortieCharacterBase::MoveRight(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		//const FRotator currentRotation = FRotator(0, GetControlRotation().Yaw, 0);
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation())*DirectionValue);
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

// ReSharper disable once CppMemberFunctionMayBeConst
void ASortieCharacterBase::Fire()
{
	EditTerrain(false, IsFiring);
}

void ASortieCharacterBase::StopFire()
{
	IsFiring = false;
}

// ReSharper disable once CppMemberFunctionMayBeConst
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
		const FVector NewGravityDirection = CameraComp->GetComponentRotation().Vector();
		USCharacterMovementComponent* GravityMovement = GetGravityMovementComponent();
		GravityMovement->SetGravityDirection(NewGravityDirection);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASortieCharacterBase::SpawnAI()
{
	if(const FHitResult HitResult = LineTraceFromCamera(); HitResult.bBlockingHit)
	{
		FTransform SpawnTransform;
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnTransform.SetLocation(HitResult.Location);

		GetWorld()->SpawnActor<ASortieAI>(SortieAI, SpawnTransform, SpawnInfo);
	}
}

void ASortieCharacterBase::EditTerrain(const bool Add, const bool ToggleAction) const
{
	if(!ToggleAction)
	{
		const FHitResult HitResult = LineTraceFromCamera();
		if(HitResult.bBlockingHit)
		{
			if (AMCChunk* ChunkHit = Cast<AMCChunk>(HitResult.GetActor()))
			{
				TArray<AMCChunk*> ChunksNeedCalculation = ChunkHit->GetNeighborChunks();

				ChunksNeedCalculation.Add(ChunkHit);

				for(AMCChunk* Chunk : ChunksNeedCalculation)
				{
					Chunk->Terraform(HitResult.ImpactPoint, SphereRadius, Add ? BrushForce : -BrushForce);
				}
			}
		}
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

FVector ASortieCharacterBase::GetActorAxisZ() const
{
	return GetGravityMovementComponent()->GetCapsuleAxisZ();
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
		EnhancedInputComponent->BindAction(SpawnAIAction, ETriggerEvent::Triggered, this, &ASortieCharacterBase::SpawnAI);
	}
}

