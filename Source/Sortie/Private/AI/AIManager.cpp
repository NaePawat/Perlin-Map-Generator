// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIManager.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAIManager::CreateAINavSystem(const FVector& ChunkLoc, const int ChunkSize, const int ChunkHeight, const float ChunkScale)
{
	const int OffsetX = ChunkLoc.X/ChunkScale;
	const int OffsetY = ChunkLoc.Y/ChunkScale;
	const int OffsetZ = ChunkLoc.Z/ChunkScale;
	
	for(int x = 0; x < ChunkSize*AIGridScaleToGridPoints; x++)
	{
		for(int y = 0; y < ChunkSize*AIGridScaleToGridPoints; y++)
		{
			for(int z = 0; z < ChunkHeight*AIGridScaleToGridPoints; z++)
			{
				FNavGrid NewGrid = {
					FVector(x*ChunkScale/AIGridScaleToGridPoints + ChunkLoc.X, y*ChunkScale/AIGridScaleToGridPoints + ChunkLoc.Y, z*ChunkScale/AIGridScaleToGridPoints+ChunkLoc.Z),
					false,
					TArray<FVector>()
				};
				
				//DrawDebugPoint(GetWorld(), NewGrid.Position, 2.f, FColor::Red, true, -1.f, 0);

				//Get nearest GridPoint for checking whether if the current coord is valid or not
				AINavGrids.Add(FVector(x+OffsetX, y+OffsetY, z+OffsetZ), NewGrid);
			}
		}
	}
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
void AAIManager::MakeAINavGrid(const FVector& MapLoc)
{
	//Create a Pathfinding grid based from the marching cubes grid points
	for(int x = 0; x < ChunkSize*AIGridScaleToGridPoints; x++)
	{
		FNavGridArray2D GridY;
		for(int y = 0; y < ChunkSize*AIGridScaleToGridPoints; y++)
		{
			FNavGridArray1D GridZ;
			for(int z = 0; z < ChunkHeight*AIGridScaleToGridPoints; z++)
			{
				FNavGrid NewGrid;
				NewGrid.Position = FVector(x*Scale/AIGridScaleToGridPoints + MapLoc.X, y*Scale/AIGridScaleToGridPoints + MapLoc.Y, z*Scale/AIGridScaleToGridPoints+MapLoc.Z);
				CheckAINavGridInvalid(NewGrid, FVector(x,y,z));
				DrawDebugPoint(GetWorld(), NewGrid.Position, 2.f, NewGrid.Invalid ? FColor::Red : FColor::Green, true, -1.f, 0);
				GridZ.Grids.Add(NewGrid);
			}
			GridY.Grids.Add(GridZ);
		}
		NavGrids.Grids.Add(GridY);
	}
}

void AAIManager::CheckAINavGridInvalid(FNavGrid CurrentGrid, const FVector& CurrentCoord) const
{
	bool Invalid = false;
	for(int x = -1; x<=1 ; ++x)
	{
		for(int y = -1; y<=1 ; ++y)
		{
			for(int z = -1; z<=1 ; ++z)
			{
				if (x == 0 && y == 0 && z == 0) continue;

				//Add nearby neighbour for the calculation
				CurrentGrid.Neighbours.Add(FVector(
					CurrentGrid.Position.X + x*Scale/AIGridScaleToGridPoints,
					CurrentGrid.Position.Y + y*Scale/AIGridScaleToGridPoints,
					CurrentGrid.Position.Z + z*Scale/AIGridScaleToGridPoints
					));

				//if the current coord is affect by any other actors
				
			}
		}
	}
	CurrentGrid.Invalid = Invalid;
}
*/
