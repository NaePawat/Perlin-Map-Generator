// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/MCChunk.h"
#include "AIManager.generated.h"

//#region Struct
struct FNavGrid
{
	FVector Position;
	bool Invalid;
	TArray<FVector> Neighbours;
};

struct FNavGridArray1D
{
	TArray<FNavGrid> Grids;
};

struct FNavGridArray2D
{
	TArray<FNavGridArray1D> Grids;
};

struct FNavGridArray3D
{
	TArray<FNavGridArray2D> Grids;
};

struct FNavBuilding
{
	FGridPointArray3D GridPoints;
	FVector ChunkLoc;
};
//#endregion

//#region Helper Class
class FAIAsyncTask : public FRunnable
{
public:
	FAIAsyncTask(AAIManager* IAim) :
	AIManager(IAim),  bIsThreadRunning(false)
	{}

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	void AddTask(const FGridPointArray3D& GridPoints, const FVector& ChunkLoc);
	bool IsRunning() const { return bIsThreadRunning; }
	void StopRunning() const { bIsThreadRunning = false; }
private:
	AAIManager* AIManager;
	FThreadSafeCounter StopTaskCounter;
	TQueue<FNavBuilding> BuildingQueue;
	FCriticalSection SyncObject;
	mutable bool bIsThreadRunning;
};

class AIGridData
{
public:
	float GScore;
	float FScore;
	FVector CameFrom;
	FVector Coord;
	float TimeToReach;

	AIGridData()
	{
		GScore = std::numeric_limits<float>::max();
		FScore = std::numeric_limits<float>::max();
		CameFrom = FVector(-1,-1,-1);
		Coord = FVector(0,0,0);
		TimeToReach = std::numeric_limits<float>::max();
	}
	
	AIGridData(const FNavGrid& NavGrid)
	{
		GScore = std::numeric_limits<float>::max();
		FScore = std::numeric_limits<float>::max();
		CameFrom = FVector(-1,-1,-1);
		Coord = NavGrid.Position;
		TimeToReach = std::numeric_limits<float>::max();
	}
};

struct FAIGridData1D
{
	TArray<AIGridData> Grids;
};

struct FAIGridData2D
{
	TArray<FAIGridData1D> Grids;
};

struct FAIGridData3D
{
	TArray<FAIGridData2D> Grids;
};
//#endregion

UCLASS()
class SORTIE_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

	//[CoordinateIndex, GridInfo]
	TMap<FVector, FNavGrid> AINavGrids;
	
	//#region AI Threading
	FAIAsyncTask* AIThread;
	FRunnableThread* CurrentRunningThread;
	//#endregion

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1), Category="3D AI Nav")
	int AIGridScaleToGridPoints = 1;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1), Category="3D AI Nav")
	float NavGridCastDistance = 100.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="3D AI Nav")
	float MaxClosestTolerance;

	UPROPERTY()
	AMCChunk* MapChunk;

	void AddToChunkUpdateQueue(const AMCChunk* Chunk) const;
	void CreateAINavSystem(const FGridPointArray3D& GridPoints, const FVector& ChunkLoc);
	FNavGrid GetClosestNavGridInfo(const FVector& DesignatedLoc);
	FNavGrid GetClosestValidNavGrid(FNavGrid& ClosestGrid, const FVector& DesiredPos, int MinDistance);

	TArray<FNavGrid> GetValidGrids();
	void DebugLogNavGrid() const;
	void DebugAINavGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category="3D AI Nav")
	TSubclassOf<AMCChunk> ChunkClass;
	
	TQueue<AMCChunk*> ChunkToUpdate;

	UPROPERTY(EditAnywhere, Category="3D AI Nav")
	TArray<AActor*> BlackListActors;

	bool CheckNavNodeInvalidRayCast(const FVector& CenterGrid) const;
	bool CheckNavNodeInvalidNeighbour(const FGridPointArray3D& GridPoints, const int X, const int Y, const int Z) const;
	TArray<FVector> GetNeighbourGrids(const FVector& DesignatedLoc, float ChunkScale) const;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
