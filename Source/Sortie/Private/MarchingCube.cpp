// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCube.h"
#include "Kismet/GameplayStatics.h"
#include "SortieCharacterBase.h"

//#region Helper Classes
FGridPoint::FGridPoint()
{
	Position = FVector(0,0,0);
	On = false;
}

FGridPoint::FGridPoint(const FVector& Position, const bool& On): Position(Position), On(On)
{
}

FGridPoint::~FGridPoint()
{
}
//#endregion

// Sets default values
AMarchingCube::AMarchingCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMarchingCube::BeginPlay()
{
	Super::BeginPlay();
	Viewer = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass()));
	CreateProceduralMarchingCubesChunk();
}

// Called every frame
void AMarchingCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMarchingCube::CreateProceduralMarchingCubesChunk()
{
	MakeGrid();
	Noise3D();
	March();
}

void AMarchingCube::MakeGrid()
{
	//add the grids (this looks ugly a little because unreal doesn't have multidimensional array!)  
	for(int x = 0; x < ChunkSize; x++)
	{
		FGridArray2D GridY;
		for(int y = 0; y < ChunkSize; y++)
		{
			FGridArray1D GridZ;
			for(int z = 0; z < ChunkHeight; z++)
			{
				GridZ.Grids.Add(FGridPoint(FVector(x*Scale, y*Scale, z*Scale), false));
			}
			GridY.Grids.Add(GridZ);
		}
		GridPoints.Grids.Add(GridY);
	}
	UE_LOG(LogTemp, Warning, TEXT("Assign Grid Pos: %s"), *GridPoints.Grids[ChunkSize-1].Grids[ChunkSize-1].Grids[ChunkHeight-1].GetPosition().ToString());
}

void AMarchingCube::Noise3D()
{
}

void AMarchingCube::March()
{
}

