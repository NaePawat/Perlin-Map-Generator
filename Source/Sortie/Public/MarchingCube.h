// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealtimeMeshActor.h"
#include "RealtimeMeshSimple.h"
#include "MarchingCube.generated.h"

//forward declaration
class ASortieCharacterBase;
class UProceduralMeshComponent;
class UMaterialInterface;

//#region Structs
struct FGridPoint
{
	FVector Position;
	float Value;
	bool On;
};

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

//#region Helper Classes
/**
 * Cube class is a combination of Grid class at 8 points to create a cube, and record the cube configuration
 */
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
	float NoiseThreshold = 0.f;//minimum value for the GridPoint to turns on

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float NoiseScale = 1.0f;//the scale of perlin noise grain
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float Scale = 1.f;// the triangle mesh's size

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float UVScale = 1.f;//the density of the UV
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int ChunkSize = 1; //on first load chunk's size

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	int ChunkHeight = 1; //on first load chunk's height

	UPROPERTY(EditAnywhere)
	int Seed = 0; //a seed for randomize the same result

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateProceduralMarchingCubesChunk();
	void MakeGridWithNoise(const FVector& MapLoc);
	void March(const FVector& MapLoc);
	void Terraform(const FVector& HitLoc, float SphereRadius, float BrushForce);

private:
	UPROPERTY()
	UProceduralMeshComponent* ProcMesh;

	/*UPROPERTY()
	URealtimeMeshComponent* RealtimeMesh;

	UPROPERTY()
	URealtimeMeshSimple* Mesh;

	FRealtimeMeshSectionKey MeshSection;*/
	
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;

	FVector InterpolateEdgePosition(const FGridPoint& CornerIndexA, const FGridPoint& CornerIndexB) const;
	static float SmoothStep(const float MinValue, const float MaxValue, const float Dist);
	
	void CreateVertex(const FGridPoint& CornerGridA, const FGridPoint& CornerGridB, const FVector& MapLoc);
	void CreateMesh();
	void UpdateMesh();
	void CleanUpData();
};
