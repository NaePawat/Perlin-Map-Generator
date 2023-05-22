// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIManager.h"
#include "AIPathFinder.generated.h"

enum class EPathFindingStatus
{
	Invalid,
	InProgress,
	Finished,
	Repath
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SORTIE_API UAIPathFinder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIPathFinder();

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f), Category="AI Stats")
	float Speed = 1.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.f), Category="AI Stats")
	float CornerSmooth = 1.f;

	UPROPERTY(EditAnywhere, Category="AI Stats")
	float UpdateFreq = 0.01;

	FNavGrid StartGrid;
	FNavGrid EndGrid;
	FVector StartLoc;
	FVector EndLoc;

	TArray<FNavGrid> TotalPaths;
	TArray<FNavGrid> CornerPoints;

	EPathFindingStatus Status = EPathFindingStatus::Finished;

	EPathFindingStatus PathFinding(const FVector& Goal, bool SuppressMovement = false);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AAIManager* AIManager;

	FTimerHandle MoveTimer;

	void Heapify(TArray<AIGridData> GridData, const int Index);
	void HeapifyDeletion(TArray<AIGridData> GridData, const int Index);
	TArray<FNavGrid> ReconstructPath(AIGridData Start, AIGridData Current, TMap<FVector, AIGridData> DataSet);

	void StartMoving();
	void MoveAIAlongPath();
	void DrawDebugPath();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
