// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIManager.h"
#include "GameFramework/Pawn.h"
#include "SortieAI.generated.h"

//forward declaration
class UAIPathFinder;
class ASortieCharacterBase;

//#region Struct
struct FStartEnd
{
	FVector Start;
	FVector End;
};
//#endregion

//#region Helper Class
class FPathFindingTask : public FRunnable
{
public:
	FPathFindingTask(UAIPathFinder* Agent):
	Finder(Agent), bIsThreadRunning(false)
	{};
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	void AddTask(const FVector& Start, const FVector& End);
	bool IsRunning() const { return bIsThreadRunning; }
	void StopRunning() const { bIsThreadRunning = false; }
	
private:
	UAIPathFinder* Finder;
	FThreadSafeCounter StopTaskCounter;
	TQueue<FStartEnd> StartEndQueue;
	mutable bool bIsThreadRunning;
};
//#endregion

UCLASS()
class SORTIE_API ASortieAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASortieAI();

	//#region AI Threading
	FPathFindingTask* AIThread;
	FRunnableThread* CurrentRunningThread;
	//#endregion

	//#region path moving
	UPROPERTY(EditAnywhere, Category="AI Stats")
	float MaxTargetGridOffset = 25.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f), Category="AI Stats")
	float Speed = 300.f;

	TArray<FNavGrid> TotalPaths;
	int TargetGridIndex;
	FVector CurrentTargetGrid;
	//#endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
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
	void MoveAIAlongPath(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
