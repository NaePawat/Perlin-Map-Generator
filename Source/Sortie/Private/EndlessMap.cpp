// Fill out your copyright notice in the Description page of Project Settings.


#include "EndlessMap.h"
#include "MarchingCube.h"
#include "Kismet/GameplayStatics.h"
#include "SortieCharacterBase.h"
#include "TerrainChunk.h"

// Sets default values
AEndlessMap::AEndlessMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEndlessMap::BeginPlay()
{
	Super::BeginPlay();

	Viewer = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass()));

	switch (GenerateChunkType)
	{
	case EGenerateType::PerlinNoise2D:
		{
			if(const ATerrainChunk* MapChunk = Cast<ATerrainChunk>(Terrain2DGen->GetDefaultObject()))
			{
				ChunkSize = MapChunk->ChunkSize - 1;
				ChunkScale = MapChunk->Scale;
				ChunkVisibleInViewDst = FMath::RoundToInt(Viewer->MaxViewDistance / ChunkSize);
			}
		}
		break;
	case EGenerateType::PerlinNoise3D:
		{
			if(const AMarchingCube* MapChunk = Cast<AMarchingCube>(Terrain3DGen->GetDefaultObject()))
			{
				ChunkSize = MapChunk->ChunkSize;
				ChunkScale = MapChunk->Scale;
				ChunkVisibleInViewDst = FMath::RoundToInt(Viewer->MaxViewDistance / ChunkSize);
			}
		}
		break;
	}
}

// Called every frame
void AEndlessMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch(GenerateChunkType)
	{
	case EGenerateType::PerlinNoise2D:
		{
			UpdateVisibleChunk2D();
		}
		break;
	case EGenerateType::PerlinNoise3D:
		{
			UpdateVisibleChunk3D();
		}
		break;
	}
}

void AEndlessMap::UpdateVisibleChunk2D()
{
	const int CurrentChunkCoordX = FMath::RoundToInt(Viewer->GetActorLocation().X / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordY = FMath::RoundToInt(Viewer->GetActorLocation().Y / (ChunkSize * ChunkScale));
	for(int YOffset = -ChunkVisibleInViewDst; YOffset <= ChunkVisibleInViewDst ; YOffset++)
	{
		for(int XOffset = -ChunkVisibleInViewDst; XOffset <=ChunkVisibleInViewDst; XOffset++)
		{
			FVector ViewedChunkCoord = FVector(CurrentChunkCoordX + XOffset, CurrentChunkCoordY + YOffset, 0);
			if (!MapChunkDict.Contains(ViewedChunkCoord))
			{
				// spawn and add the map chunk
				FTransform SpawnTransform;
				FActorSpawnParameters SpawnInfo;
				SpawnTransform.SetLocation(FVector(ViewedChunkCoord.X*ChunkSize*ChunkScale, ViewedChunkCoord.Y*ChunkSize*ChunkScale,0));
				
				ATerrainChunk* NewMapChunk = GetWorld()->SpawnActor<ATerrainChunk>(Terrain2DGen, SpawnTransform,SpawnInfo);
				NewMapChunk->ChunkCoord = ViewedChunkCoord;
				
				MapChunkDict.Add(ViewedChunkCoord, NewMapChunk);
			}
		}
	}
}

void AEndlessMap::UpdateVisibleChunk3D()
{
	//TODO: Implement the limit height
	const int CurrentChunkCoordX = FMath::RoundToInt(Viewer->GetActorLocation().X / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordY = FMath::RoundToInt(Viewer->GetActorLocation().Y / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordZ = FMath::RoundToInt(Viewer->GetActorLocation().Z / (ChunkSize* ChunkScale));

	//three for loop
	for(int ZOffset = -ChunkVisibleInViewDst; ZOffset <= ChunkVisibleInViewDst; ZOffset++)
	{
		for(int YOffset = -ChunkVisibleInViewDst; YOffset <= ChunkVisibleInViewDst; YOffset++)
		{
			for(int XOffset = -ChunkVisibleInViewDst; XOffset <= ChunkVisibleInViewDst; XOffset++)
			{
				FVector ViewedChunkCoord = FVector(CurrentChunkCoordX + XOffset, CurrentChunkCoordY + YOffset, CurrentChunkCoordZ + ZOffset);
				if(!MapChunkDict.Contains(ViewedChunkCoord))
				{
					// spawn and add the map chunk
					FTransform SpawnTransform;
					FActorSpawnParameters SpawnInfo;
					SpawnTransform.SetLocation(FVector(
						ViewedChunkCoord.X*ChunkSize*ChunkScale - ChunkScale*ViewedChunkCoord.X,
						ViewedChunkCoord.Y*ChunkSize*ChunkScale - ChunkScale*ViewedChunkCoord.Y,
						ViewedChunkCoord.Z*ChunkSize*ChunkScale - ChunkScale*ViewedChunkCoord.Z));

					AMarchingCube* NewMapChunk = GetWorld()->SpawnActor<AMarchingCube>(Terrain3DGen, SpawnTransform, SpawnInfo);
					NewMapChunk->ChunkCoord = ViewedChunkCoord;

					MapChunkDict.Add(ViewedChunkCoord, NewMapChunk);
				}
			}
		}
	}
}

