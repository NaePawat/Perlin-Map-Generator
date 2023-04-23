// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarchingCube.generated.h"

//forward declaration
class ASortieCharacterBase;

//#region Helper Classes
class FGridPoint
{
public:
	//Constructor
	FGridPoint();
	FGridPoint(const FVector& Position, const bool& On);
	~FGridPoint();
	
protected:
	FVector Position;
	bool On;

public:
	FVector GetPosition() const
	{
		return Position;
	}

	bool GetOn() const
	{
		return On;
	}

	void SetPosition(const FVector& NewPosition)
	{
		Position = NewPosition;
	}

	void SetOn(const bool& NewOnState)
	{
		On = NewOnState;
	}
};
//#endregion

//#region Structs
struct FGridArray1D
{
	TArray<FGridPoint> Grids;
};

struct FGridArray2D
{
	TArray<FGridArray1D> Grids;
};

struct FGridArray3D
{
	TArray<FGridArray2D> Grids;
};
//#endregion

UCLASS()
class SORTIE_API AMarchingCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMarchingCube();

protected:
	UPROPERTY()
	ASortieCharacterBase* Viewer;

	FGridArray3D GridPoints;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float NoiseThreshold = 0.f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float Scale = 1.f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int ChunkSize = 1;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	int ChunkHeight = 1;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateProceduralMarchingCubesChunk();
	void MakeGrid();
	void Noise3D();
	void March();
};
