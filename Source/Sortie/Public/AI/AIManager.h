// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1), Category="3D AI Nav")
	int AIGridScaleToGridPoints = 1;

	void CreateAINavSystem(const FVector& ChunkLoc, int ChunkSize, int ChunkHeight, float ChunkScale);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="3D AI Nav")
	TArray<AActor*> BlackListActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
