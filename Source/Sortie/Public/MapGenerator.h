// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapGenerator.generated.h"

//forward declaration
class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class SORTIE_API AMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int XSize = 0; //init x-axis number of squares

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int YSize = 0; //init y-axis number of squares

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float ZMultiplier = 1.0f; //amplitude enhance of the height map

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	float FlatLandThreshold = 1.0f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float NoiseScale = 1.0f; //the scale of perlin noise grain

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	int Octaves = 1; // number of noise maps before each of them are added together

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float Persistance = 0.0f; // control decrease in amplitude of the octave

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0))
	float Lacunarity = 0.0f; //control increase in frequency of the octave
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float Scale = 0; //the triangle mesh's size

	UPROPERTY(EditAnywhere, meta=(ClampMin = 0.000001))
	float UVScale = 0; 

	UPROPERTY(EditAnywhere)
	int Seed = 0;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material; // material applied to the generated mesh
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV0;

	void CreateVertices();
	void CreateTriangles();
};
