// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIManager.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAIManager::CreateAINavSystem(const FGridPointArray3D& GridPoints, const FVector& ChunkLoc, const int ChunkSize, const int ChunkHeight, const float ChunkScale)
{
	//Clear any existing NavGrid (prevent allocate crash)
	
	const int OffsetX = ChunkLoc.X/ChunkScale;
	const int OffsetY = ChunkLoc.Y/ChunkScale;
	const int OffsetZ = ChunkLoc.Z/ChunkScale;
	
	for(int x = 0; x < ChunkSize*AIGridScaleToGridPoints; x++)
	{
		for(int y = 0; y < ChunkSize*AIGridScaleToGridPoints; y++)
		{
			for(int z = 0; z < ChunkHeight*AIGridScaleToGridPoints; z++)
			{
				//Current AI Grid Loc
				const FVector CurrentPosition = FVector(x*ChunkScale/AIGridScaleToGridPoints + ChunkLoc.X, y*ChunkScale/AIGridScaleToGridPoints + ChunkLoc.Y, z*ChunkScale/AIGridScaleToGridPoints+ChunkLoc.Z);

				//Check the nearest Actor to determine whether there's a surface or not
				const auto [Position, Value, On] = GetClosestGridInfo(GridPoints, CurrentPosition, ChunkScale);

				//Check other neighbour of the grid, are there any obstacle?
				const bool Invalid = On ? true : CheckNavNodeInvalid(Position);

				//Add the neighbours
				//Just FVector position is enough and we'll look up in the TMap later
				const TArray<FVector> NeighbourArray = GetNeighbourGrids(CurrentPosition, ChunkScale);
				
				FNavGrid NewGrid = {
					CurrentPosition,
					Invalid,
					NeighbourArray
				};

				DrawDebugPoint(GetWorld(), NewGrid.Position, 2.f, NewGrid.Invalid ? FColor::Red : FColor::Green, true, -1.f, 0);

				if(AINavGrids.Contains(FVector(x+OffsetX, y+OffsetY, z+OffsetZ)))
				{
					AINavGrids[FVector(x+OffsetX, y+OffsetY, z+OffsetZ)] = NewGrid;
				}
				else
				{
					AINavGrids.Add(FVector(x+OffsetX, y+OffsetY, z+OffsetZ), NewGrid);
				}
			}
		}
	}
}

FGridPoint AAIManager::GetClosestGridInfo(const FGridPointArray3D& GridPoints, const FVector& DesignatedLoc, float ChunkScale) const
{
	return GridPoints.
		Grids[FMath::CeilToInt(DesignatedLoc.X/ChunkScale)].
		Grids[FMath::CeilToInt(DesignatedLoc.Y/ChunkScale)].
		Grids[FMath::FloorToInt(DesignatedLoc.Z/ChunkScale)];
}

bool AAIManager::CheckNavNodeInvalid(const FVector& CenterGrid) const
{
	//To check if there are any obstacles, we're gonna do a little raycast to each direction (hopefully this doesn't take to much power lol)
	//This is not only for the terrain that we're gonna check, but also all the objects that can block the navigation path
	//Hard code for optimization
	FHitResult HitResult;
	
	FVector TraceStart = CenterGrid;
	FVector TraceEnd;

	for(int x = -1; x<=1 ; ++x)
	{
		for(int y = -1; y<=1 ; ++y)
		{
			for(int z = -1; z<=1 ; ++z)
			{
				if (x == 0 && y == 0 && z == 0) continue;
				FCollisionQueryParams QueryParams;
				TraceEnd = TraceStart + FVector(x*NavGridCastDistance, y*NavGridCastDistance, z*NavGridCastDistance);
				GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
				//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, HitResult.bBlockingHit ? FColor::Orange : FColor::Magenta, true, -1.f, 0, 1.0f);

				if(HitResult.bBlockingHit) return false;
			}
		}
	}

	return true;
}

TArray<FVector> AAIManager::GetNeighbourGrids(const FVector& DesignatedLoc, float ChunkScale) const
{
	//Just FVector position is enough and we'll look up in the TMap later
	//Hard code for optimization
	TArray<FVector> NeighbourArray;
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));

	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));

	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y - ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z - ChunkScale/AIGridScaleToGridPoints));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z));
	NeighbourArray.Add(FVector(DesignatedLoc.X + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Y + ChunkScale/AIGridScaleToGridPoints,DesignatedLoc.Z + ChunkScale/AIGridScaleToGridPoints));

	return NeighbourArray;
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
