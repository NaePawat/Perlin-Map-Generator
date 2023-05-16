// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainChunk.generated.h"

//forward declaration
class ASortieCharacterBase;
class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class SORTIE_API ATerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainChunk();

protected:

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float ZMultiplier = 1.0f; //amplitude enhance of the height map

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	float FlatLandThreshold = 1.0f; //The minimum amplitude to escape from flattening

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float NoiseScale = 1.0f; //the scale of perlin noise grain

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int Octaves = 1; // number of noise maps before each of them are added together

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float Persistance = 0.0f; // control decrease in amplitude of the octave

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float Lacunarity = 0.0f; //control increase in frequency of the octave

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float UVScale = 0; //the density of the UV

	UPROPERTY(EditAnywhere)
	int Seed = 0; //a seed for randomize the same result
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material; // material applied to the generated mesh

	UPROPERTY()
	ASortieCharacterBase* Viewer;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float Scale = 0; //the triangle mesh's size
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0, ClampMax = 241))
	int ChunkSize = 241; //number of squares matrix (recommend base 2 divisible)

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0, ClampMax = 6))
	int LOD = 0;

	FVector ChunkCoord;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateProceduralTerrainChunk();
	void CheckVisible();
	void SetVisible(bool Visible);

private:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	void CreateVertices();
	void CreateTriangles(); 
};
