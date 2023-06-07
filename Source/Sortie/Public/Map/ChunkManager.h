// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkManager.generated.h"

//forward declaration
class ATerrainChunk;
class AMCChunk;
class ASortieCharacterBase;

UENUM()
enum class EGenerateType: uint8
{
	PerlinNoise2D = 0 UMETA(DisplayName = "2D PerlinNoise Terrain"),
	PerlinNoise3D = 1 UMETA(DisplayName = "3D PerlinNoise Terrain")
};

UCLASS()
class SORTIE_API AChunkManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkManager();

protected:

	UPROPERTY(EditAnywhere, Category="Chunk Generation")
	int SpawnSize = 1;

	UPROPERTY(EditAnywhere, Category="Chunk Generation")
	int SpawnHeight = 1;

	UPROPERTY(EditAnywhere, Category="Chunk Generation")
	int MaxSpawnEndless;

	UPROPERTY(EditAnywhere, Category="Chunk Components")
	bool Endless = false;

	UPROPERTY(BlueprintReadOnly, Category="Chunk Components")
	ASortieCharacterBase* Viewer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chunk Components")
	EGenerateType GenerateChunkType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chunk Components")
	TSubclassOf<ATerrainChunk> Terrain2DGen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chunk Components")
	TSubclassOf<AMCChunk> Terrain3DGen;

	int ChunkSize = 0;
	int ChunkHeight = 0;
	float ChunkScale = 0;
	int ChunkVisibleInViewDst = 0;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitialSpawn2D();
	void InitialSpawn3D();
	void UpdateVisibleChunk2D();
	void UpdateVisibleChunk3D();
	AMCChunk* Spawn3D(const FVector& ChunkCoord) const;

public:

	UPROPERTY()
	TMap<FVector, AActor*> MapChunkDict;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
