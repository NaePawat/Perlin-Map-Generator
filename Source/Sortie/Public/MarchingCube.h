// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarchingCube.generated.h"

//forward declaration
class ASortieCharacterBase;
class UProceduralMeshComponent;
class UMaterialInterface;

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

class FCube
{
	//The cube that we're creating, it should looks like this:
	/*      4 ------ 5
	 *      |        |
	 *      | 7 ------- 6
	 *      | |      |  |
	 *      0 | ---- 1  |
	 *        |         |
	 *        3 ------- 2
	*/
	
public:
	//Constructor
	FCube();
	FCube(
		const FGridPoint& PosZero,
		const FGridPoint& PosOne,
		const FGridPoint& PosTwo,
		const FGridPoint& PosThree,
		const FGridPoint& PosFour,
		const FGridPoint& PosFive,
		const FGridPoint& PosSix,
		const FGridPoint& PosSeven
		);
	~FCube();

	TArray<FGridPoint> Points;
	int Config = 0;
	int CalculateConfig();
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

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float NoiseScale = 1.0f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float Scale = 1.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float UVScale = 1.f;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int ChunkSize = 1;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	int ChunkHeight = 1;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateProceduralMarchingCubesChunk();
	void MakeGridWithNoise();
	void March();

private:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	void CreateVertex(const FVector& CornerIndexA, const FVector& CornerIndexB);
};
