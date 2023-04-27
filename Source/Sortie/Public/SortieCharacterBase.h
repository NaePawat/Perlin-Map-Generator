// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "SortieCharacterBase.generated.h"

//forward declaration
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class SORTIE_API ASortieCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASortieCharacterBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USpringArmComponent* SpringArmComp;

	//#region Player Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputMappingContext* PlayerMappingContext; //let's forward declare this because it's a pointer

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* FireAction;

	bool IsFiring = false;
	
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire();
	void StopFire();
	//#endregion

	//#region Terrain Editing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain Editing")
	float LineTraceDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain Editing")
	float SphereRadius = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Terrain Editing")
	float BrushForce = 0.1f;
	//#endregion
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View Helper")
	float MaxViewDistance = 300.f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
