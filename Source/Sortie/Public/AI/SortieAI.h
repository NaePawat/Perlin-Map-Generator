// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SortieAI.generated.h"

//forward declaration
class UAIPathFinder;
class ASortieCharacterBase;

UCLASS()
class SORTIE_API ASortieAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASortieAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Target to move to
	UPROPERTY(EditAnywhere, Category="PathFinding")
	ASortieCharacterBase* Target;
	
	UPROPERTY(EditAnywhere, Category="PathFinding")
	float Interval = 0.1;

	UPROPERTY()
	UAIPathFinder* Agent;
	
	FTimerHandle MoveTimerHandle;

	FVector PrevTargetLoc;

	void MoveFromAToB();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
