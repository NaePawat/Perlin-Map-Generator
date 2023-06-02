// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HAL/ThreadSafeBool.h"
#include "RealtimeMeshActor.h"
#include "RealtimeMeshSimple.h"
#include "MCChunk.generated.h"

//forward declaration
class AAIManager;
class AEndlessMap;
class ASortieCharacterBase;
class UMaterialInterface;

//#region Structs
struct FGridPoint
{
	FVector Position;
	float Value;
	bool On;
};

struct FGridPointArray1D
{
	TArray<FGridPoint> Grids;
};

struct FGridPointArray2D
{
	TArray<FGridPointArray1D> Grids;
};

struct FGridPointArray3D
{
	TArray<FGridPointArray2D> Grids;
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

UENUM()
enum class EChunkType: uint8
{
	PerlinNoise = 0 UMETA(DisplayName = "Perlin Noise"),
	PerlinWorm = 1 UMETA(DisplayName = "Perlin Worm")
};


UCLASS()
class SORTIE_API AMCChunk : public ARealtimeMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMCChunk();

protected:
	UPROPERTY()
	ASortieCharacterBase* Viewer;
	
	UPROPERTY()
	AAIManager* AIManager;

	UPROPERTY()
	AEndlessMap* EndlessMap;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting Generation")
	EChunkType ChunkType;
	
	//#region MarchingCubes
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	int Octaves = 1; // number of noise maps before each of them are added together

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	float Persistance = 0.0f; // control decrease in amplitude of the octave

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	float Lacunarity = 0.0f; //control increase in frequency of the octave
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001), Category="Marching Cubes")
	float NoiseThreshold = 0.f;//minimum value for the GridPoint to turns on

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	float NoiseScale = 1.0f;//the scale of perlin noise grain
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001), Category="Marching Cubes")
	float Scale = 1.f;// the triangle mesh's size

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001), Category="Marching Cubes")
	float UVScale = 1.f;//the density of the UV
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	int ChunkSize = 1; //on first load chunk's size
	UPROPERTY(EditAnywhere, meta=(ClampMin = 1), Category="Marching Cubes")
	int ChunkHeight = 1; //on first load chunk's height

	UPROPERTY(EditAnywhere, Category="Marching Cubes")
	int Seed = 0; //a seed for randomize the same result

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0), Category="Marching Cubes")
	int LOD = 1;

	UPROPERTY(EditAnywhere, Category="Marching Cubes")
	UMaterialInterface* Material;

	// std::vector<std::vector<std::vector<bool>>> visited(sizeX, std::vector<std::vector<bool>>(sizeY, std::vector<bool>(sizeZ, false)));
	FGridPointArray3D GridPoints;
	FVector ChunkCoord;

	void CreateProceduralMarchingCubesChunk();
	void MakeGrid(const FVector& MapLoc, bool WithNoise = true);
	void March(const FVector& MapLoc);
	void Terraform(const FVector& HitLoc, float SphereRadius, float BrushForce);
	static float SmoothStep(const float MinValue, const float MaxValue, const float Dist);
	//#endregion

	//#region Perlin Worm
	UPROPERTY(EditAnywhere, Category="Perlin Worm")
	int MaxWorms = 7;

	UPROPERTY(EditAnywhere, Category="Perlin Worm")
	int MaxWormLength = 150;

	UPROPERTY(EditAnywhere, Category="Perlin Worm")
	int MinWormLength = 20;

	UPROPERTY(EditAnywhere, Category="Perlin Worm")
	int MaxWormRadius;
	
	UPROPERTY(EditAnywhere, Category="Perlin Worm")
	int MinWormRadius;

	TArray<AMCChunk*> WormifyChunk(const FVector& ChunkLoc);
	//#endregion

	//#region Helpers
	TArray<AMCChunk*> GetNeighborChunks() const;

	FTimerHandle ChunkTimerHandle;
	//#endregion

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	URealtimeMeshComponent* RealtimeMesh;

	UPROPERTY()
	URealtimeMeshSimple* Mesh;

	FRealtimeMeshSectionKey MeshSection;
	
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;

	FVector InterpolateEdgePosition(const FGridPoint& CornerIndexA, const FGridPoint& CornerIndexB) const;
	
	void CreateVertex(const FGridPoint& CornerGridA, const FGridPoint& CornerGridB, const FVector& MapLoc);
	void CreateProcMesh();
	void UpdateProcMesh();
	void UpdateAINavigation() const;
	void CleanUpData();
};
