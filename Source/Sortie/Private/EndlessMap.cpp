// Fill out your copyright notice in the Description page of Project Settings.


#include "EndlessMap.h"
#include "Kismet/GameplayStatics.h"
#include "MapGenerator.h"
#include "SortieCharacterBase.h"

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
	if(!Viewer)
	{
		UE_LOG(LogTemp, Warning, TEXT("No character was spawn at player start!"));
	}
	
	if(const AMapGenerator* MapChunk = Cast<AMapGenerator>(MapGen->GetDefaultObject()))
	{
		ChunkSize = MapChunk->ChunkSize - 1;
		ChunkScale = MapChunk->Scale;
		ChunkVisibleInViewDst = FMath::RoundToInt(Viewer->MaxViewDistance / ChunkSize);
		UE_LOG(LogTemp, Warning, TEXT("Infinite Map ChunkSize: %d"), ChunkSize);
		UE_LOG(LogTemp, Warning, TEXT("Infinite Map ChunkScale: %f"), ChunkScale);
		UE_LOG(LogTemp, Warning, TEXT("Infinite Map ChunkVisibleDst: %d"), ChunkVisibleInViewDst);
	}

	//For debugging only
	UpdateVisibleChunk();
}

// Called every frame
void AEndlessMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateVisibleChunk();
}

void AEndlessMap::UpdateVisibleChunk()
{
	const int CurrentChunkCoordX = FMath::RoundToInt(Viewer->GetActorLocation().X / (ChunkSize * ChunkScale));
	const int CurrentChunkCoordY = FMath::RoundToInt(Viewer->GetActorLocation().Y / (ChunkSize * ChunkScale));
	UE_LOG(LogTemp, Warning, TEXT("Actor Location: %f %f"), Viewer->GetActorLocation().X, Viewer->GetActorLocation().Y);

	for(int YOffset = -ChunkVisibleInViewDst; YOffset <= ChunkVisibleInViewDst ; YOffset++)
	{
		for(int XOffset = -ChunkVisibleInViewDst; XOffset <=ChunkVisibleInViewDst; XOffset++)
		{
			FVector2D ViewedChunkCoord = FVector2D(CurrentChunkCoordX + XOffset, CurrentChunkCoordY + YOffset);
			if (MapChunkDict.Contains(ViewedChunkCoord))
			{
				//If the view chunk is already added in the dict
			} else
			{
				// spawn and add the map chunk
				FTransform SpawnTransform;
				FActorSpawnParameters SpawnInfo;
				SpawnTransform.SetLocation(FVector(ViewedChunkCoord.X*ChunkSize*ChunkScale, ViewedChunkCoord.Y*ChunkSize*ChunkScale,0));
				AMapGenerator* NewMapChunk = GetWorld()->SpawnActor<AMapGenerator>(MapGen, SpawnTransform,SpawnInfo);
				
				UE_LOG(LogTemp, Warning, TEXT("Spawn Loc: %d %d"), static_cast<int>(ViewedChunkCoord.X * ChunkSize * ChunkScale), static_cast<int>(ViewedChunkCoord.Y * ChunkSize * ChunkScale));
				MapChunkDict.Add(ViewedChunkCoord, NewMapChunk);
			}
		}
	}
}

