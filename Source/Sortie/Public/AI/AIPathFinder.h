// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIManager.h"
#include "AIPathFinder.generated.h"

//forward declaration
class AMCChunk;
class ASortieAI;

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
	float CornerSmooth = 1.f;

	FNavGrid StartGrid;
	FNavGrid EndGrid;
	FVector StartLoc;
	FVector EndLoc;

	//#region path moving
	TArray<FNavGrid> CornerPoints;
	//#endregion

	EPathFindingStatus Status = EPathFindingStatus::Finished;

	EPathFindingStatus PathFinding(const FVector& Start, const FVector& Goal, bool SuppressMovement = false);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Pathfinding")
	AAIManager* AIManager;

	UPROPERTY()
	ASortieAI* SortieAI;

	FTimerHandle MoveTimer;

	static void HeapModify(TArray<AIGridData> GridData, const int Index);
	static void HeapModifyDeletion(TArray<AIGridData> GridData, const int Index);
	TArray<FNavGrid> ReconstructPath(AIGridData Start, AIGridData Current, TMap<FVector, AIGridData> DataSet);

	void CleanPathFindingData();
	void DrawDebugPath();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
