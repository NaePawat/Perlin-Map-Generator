// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndlessMap.generated.h"

//forward declaration
class AMapGenerator;
class ASortieCharacterBase;

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
	TSubclassOf<AMapGenerator> MapGen;

	int ChunkSize = 0;
	float ChunkScale = 0;
	int ChunkVisibleInViewDst = 0;
	TMap<FVector2D, AMapGenerator*> MapChunkDict;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateVisibleChunk();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
