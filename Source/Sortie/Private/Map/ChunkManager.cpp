// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/ChunkManager.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"
#include "Map/MCChunk.h"
#include "Map/TerrainChunk.h"
#include "SortieCharacterBase.h"

// Sets default values
AChunkManager::AChunkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChunkManager::BeginPlay()
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
				if (Endless)
				{
					ChunkVisibleInViewDst = FMath::RoundToInt(Viewer->MaxViewDistance / ChunkSize);
				}
				else
				{
					InitialSpawn2D();
				}
			}
		}
		break;
	case EGenerateType::PerlinNoise3D:
		{
			if(const AMCChunk* MapChunk = Cast<AMCChunk>(Terrain3DGen->GetDefaultObject()))
			{
				ChunkSize = MapChunk->ChunkSize;
				ChunkHeight = MapChunk->ChunkHeight;
				ChunkScale = MapChunk->Scale;
				if (Endless)
				{
					ChunkVisibleInViewDst = FMath::RoundToInt(Viewer->MaxViewDistance / ChunkSize);
				}
				else
				{
					InitialSpawn3D();
				}
			}
		}
		break;
	}
}

void AChunkManager::InitialSpawn2D()
{
	for(int x = 0; x<SpawnSize; x++)
	{
		for(int y=0; y<SpawnSize; y++)
		{
			// spawn and add the map chunk
			FTransform SpawnTransform;
			FActorSpawnParameters SpawnInfo;
			SpawnTransform.SetLocation(FVector(x*ChunkSize*ChunkScale, y*ChunkSize*ChunkScale,0));
				
			ATerrainChunk* NewMapChunk = GetWorld()->SpawnActor<ATerrainChunk>(Terrain2DGen, SpawnTransform,SpawnInfo);
			NewMapChunk->ChunkCoord = FVector(x,y,0);
				
			MapChunkDict.Add(FVector(x,y,0), NewMapChunk);
		}
	}
}

void AChunkManager::InitialSpawn3D()
{
	TArray<AMCChunk*> Chunks;
	for(int x = 0; x<SpawnSize; x++)
	{
		for(int y=0; y<SpawnSize; y++)
		{
			for(int z=0; z<SpawnHeight; z++)
			{
				const FVector CurrentCoord = FVector(x,y,z);
				AMCChunk* NewMapChunk = Spawn3D(CurrentCoord);
				MapChunkDict.Add(CurrentCoord, NewMapChunk);
				Chunks.Add(NewMapChunk);
				NewMapChunk->ChunkType == EChunkType::PerlinNoise ?
					NewMapChunk->MakeGrid(NewMapChunk->GetActorLocation()) :
					NewMapChunk->MakeGrid(NewMapChunk->GetActorLocation(), false);
			}
		}
	}
	
	for(AMCChunk* Chunk : Chunks)
	{
		Chunk->CreateProceduralMarchingCubesChunk();
	}
}

// Called every frame
void AChunkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Endless)
	{
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
}

void AChunkManager::UpdateVisibleChunk2D()
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

void AChunkManager::UpdateVisibleChunk3D()
{
	//TODO: Implement the limit height
	const int CurrentChunkCoordX = FMath::RoundToInt(Viewer->GetActorLocation().X / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordY = FMath::RoundToInt(Viewer->GetActorLocation().Y / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordZ = FMath::RoundToInt(Viewer->GetActorLocation().Z / (ChunkHeight* ChunkScale));

	for(int ZOffset = -ChunkVisibleInViewDst; ZOffset <= ChunkVisibleInViewDst; ZOffset++)
	{
		for(int YOffset = -ChunkVisibleInViewDst; YOffset <= ChunkVisibleInViewDst; YOffset++)
		{
			for(int XOffset = -ChunkVisibleInViewDst; XOffset <= ChunkVisibleInViewDst; XOffset++)
			{
				FVector ViewedChunkCoord = FVector(CurrentChunkCoordX + XOffset, CurrentChunkCoordY + YOffset, CurrentChunkCoordZ + ZOffset);
				if(!MapChunkDict.Contains(ViewedChunkCoord))
				{
					AsyncTask(ENamedThreads::GameThread, [=]
					{
						// spawn and add the map chunk
						AMCChunk* NewMapChunk = Spawn3D(ViewedChunkCoord);
						NewMapChunk->ChunkCoord = ViewedChunkCoord;
						MapChunkDict.Add(ViewedChunkCoord, NewMapChunk);
						NewMapChunk->CreateProceduralMarchingCubesChunk();
					});
				}
			}
		}
	}
}

AMCChunk* AChunkManager::Spawn3D(const FVector& ChunkCoord) const
{
	// spawn and add the map chunk
	FTransform SpawnTransform;
	const FActorSpawnParameters SpawnInfo;
	SpawnTransform.SetLocation(FVector(
		ChunkCoord.X*ChunkSize*ChunkScale - ChunkScale*ChunkCoord.X,
		ChunkCoord.Y*ChunkSize*ChunkScale - ChunkScale*ChunkCoord.Y,
		ChunkCoord.Z*ChunkSize*ChunkScale - ChunkScale*ChunkCoord.Z));

	AMCChunk* NewMapChunk = GetWorld()->SpawnActor<AMCChunk>(Terrain3DGen, SpawnTransform, SpawnInfo);
	NewMapChunk->ChunkCoord = ChunkCoord;

	return NewMapChunk;
}

