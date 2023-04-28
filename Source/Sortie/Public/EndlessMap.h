// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndlessMap.generated.h"

//forward declaration
class ATerrainChunk;
class AMarchingCube;
class ASortieCharacterBase;

UENUM()
enum class EGenerateType: uint8
{
	PerlinNoise2D = 0 UMETA(DisplayName = "2D PerlinNoise Terrain"),
	PerlinNoise3D = 1 UMETA(DisplayName = "3D PerlinNoise Terrain")
};

UCLASS()
class SORTIE_API AEndlessMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndlessMap();

protected:

	UPROPERTY(BlueprintReadOnly, Category="Endless Components")
	ASortieCharacterBase* Viewer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Endless Components")
	EGenerateType GenerateChunkType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Endless Components")
	TSubclassOf<ATerrainChunk> Terrain2DGen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Endless Components")
	TSubclassOf<AMarchingCube> Terrain3DGen;

	int ChunkSize = 0;
	float ChunkScale = 0;
	int ChunkVisibleInViewDst = 0;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateVisibleChunk2D();
	void UpdateVisibleChunk3D();

public:

	UPROPERTY()
	TMap<FVector, AActor*> MapChunkDict;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
