// Fill out your copyright notice in the Description page of Project Settings.


#include "SortieCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASortieCharacterBase::ASortieCharacterBase()
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
	}

}

