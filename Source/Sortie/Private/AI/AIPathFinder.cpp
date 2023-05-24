// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIPathFinder.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAIPathFinder::UAIPathFinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAIPathFinder::BeginPlay()
{
	Super::BeginPlay();
	AIManager = Cast<AAIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAIManager::StaticClass()));
	// ...
	
}

// Called every frame
void UAIPathFinder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//Heap Optimization
void UAIPathFinder::HeapModify(TArray<AIGridData> GridData, const int Index)
{
	const int Parent = (Index - 1)/2;
	if(Parent > -1)
	{
		if(GridData[Index].FScore < GridData[Parent].FScore)
		{
			const AIGridData Aom = GridData[Index];
			GridData[Index] = GridData[Parent];
			GridData[Parent] = Aom;
			HeapModify(GridData, Parent);
		}
	}
}

//Heap Optimization
void UAIPathFinder::HeapModifyDeletion(TArray<AIGridData> GridData, const int Index)
{
	int Smallest = Index;
	const int Left = 2 * Index + 1;
	const int Right = 2 * Index + 2;

	if (Left < GridData.Num() && GridData[Left].FScore < GridData[Smallest].FScore)
	{
		Smallest = Left;
	}
	if (Right < GridData.Num() && GridData[Right].FScore < GridData[Smallest].FScore)
	{
		Smallest = Right;
	}
	if (Smallest != Index)
	{
		AIGridData Aom = GridData[Index];
		GridData[Index] = GridData[Smallest];
		GridData[Smallest] = Aom;

		// Recursively heap modify the affected sub-tree
		HeapModifyDeletion(GridData, Smallest);
	}
}

TArray<FNavGrid> UAIPathFinder::ReconstructPath(AIGridData Start, AIGridData Current, TMap<FVector, AIGridData> DataSet)
{
	CornerPoints = TArray<FNavGrid>();
	TArray<FNavGrid> TotPaths;

	FNavGrid CurrentGrid = AIManager->AINavGrids[Current.Coord / AIManager->MapChunk->Scale];
	TotPaths.Add(CurrentGrid);

	FNavGrid CameFromGrid = AIManager->AINavGrids[Current.CameFrom / AIManager->MapChunk->Scale];

	FVector Direction = CurrentGrid.Position - CameFromGrid.Position;
	Direction.Normalize();

	CornerPoints.Add(CurrentGrid);

	//UE_LOG(LogTemp, Warning, TEXT("ReconstructPath CurrentGrid: %s"), *Current.Coord.ToString());
	
	//int Count = 0;
	while (CameFromGrid.Position != Start.Coord)
	{
		CurrentGrid = AIManager->AINavGrids[Current.Coord / AIManager->MapChunk->Scale];
		CameFromGrid = AIManager->AINavGrids[Current.CameFrom / AIManager->MapChunk->Scale];

		FVector Dir = CurrentGrid.Position - CameFromGrid.Position;
		Dir.Normalize();
		if(Dir != Direction)
		{
			CornerPoints.Add(CurrentGrid);
			Direction = Dir;
		}

		TotPaths.Add(CameFromGrid);
		Current = DataSet[Current.CameFrom];
	}

	CurrentGrid = AIManager->AINavGrids[Current.Coord / AIManager->MapChunk->Scale];
	CornerPoints.Add(CurrentGrid);

	return TotPaths;
}

EPathFindingStatus UAIPathFinder::PathFinding(const FVector& Goal, bool SuppressMovement)
{
	const FVector CurrentLoc = GetOwner()->GetActorLocation();
	StartLoc = CurrentLoc;
	EndLoc = Goal;
	StartGrid = AIManager->GetClosestNavGridInfo(CurrentLoc);
	EndGrid = AIManager->GetClosestNavGridInfo(Goal);
	//UE_LOG(LogTemp, Warning, TEXT("StartLoc: %s %d EndLoc %s %d"), *StartGrid.Position.ToString(), StartGrid.Invalid, *EndGrid.Position.ToString(), EndGrid.Invalid);
	DrawDebugPoint(GetWorld(), StartGrid.Position, 20.f, StartGrid.Invalid ? FColor::Red : FColor::Green, true, 1.f, 0);
	DrawDebugPoint(GetWorld(), EndGrid.Position, 20.f, EndGrid.Invalid ? FColor::Red : FColor::Green, true, 1.f, 0);

	if(StartGrid.Position == EndGrid.Position || StartGrid.Invalid || EndGrid.Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pathfinding failed: Start or End point not valid"));
		Status = EPathFindingStatus::Invalid;
		return Status;
	}

	
	TMap<FVector, AIGridData> DataSet;
	TArray<AIGridData> OpenSet;

	AIGridData StartingGrid = AIGridData(StartGrid);
	DataSet.Add(StartingGrid.Coord, StartingGrid);
	StartingGrid.GScore = 0;
	StartingGrid.TimeToReach = 0;
	OpenSet.Add(StartingGrid);
	UE_LOG(LogTemp,Warning, TEXT("Starting Grid: %s"), *StartingGrid.Coord.ToString());

	while (OpenSet.Num() > 0)
	{
		AIGridData Current = OpenSet[0];
		//UE_LOG(LogTemp,Warning, TEXT("Current: %s, CameFrom: %s"), *Current.Coord.ToString(), *Current.CameFrom.ToString());
		//DrawDebugPoint(GetWorld(), Current.Coord, 4.f, FColor::Green, true, -1.f, 0);
		if(Current.Coord == EndGrid.Position)
		{
			UE_LOG(LogTemp, Warning, TEXT("Calculated until the target"));
			TotalPaths = ReconstructPath(StartingGrid, Current, DataSet);
			UE_LOG(LogTemp,Warning, TEXT("TotalPaths: %d"), TotalPaths.Num());
			if(!SuppressMovement)
			{
				Status = EPathFindingStatus::InProgress;
				StartMoving();
			}

			return Status;
		}

		OpenSet.RemoveAt(0);
		HeapModifyDeletion(OpenSet, 0);

		FNavGrid CurrentGrid = AIManager->AINavGrids[Current.Coord / AIManager->MapChunk->Scale];

		for(int i = 0; i<CurrentGrid.Neighbours.Num(); i++)
		{
			if(AIManager->AINavGrids.Contains(CurrentGrid.Neighbours[i]))
			{
				FNavGrid NeighbourGrid = AIManager->AINavGrids[CurrentGrid.Neighbours[i]];
				AIGridData Neighbour;
				bool bNeighbourPassed = true;

				//Neighbour is not in the dataset yet
				if(!DataSet.Contains(NeighbourGrid.Position))
				{
					Neighbour = AIGridData(NeighbourGrid);
					DataSet.Add(Neighbour.Coord, Neighbour);
					bNeighbourPassed = false;
				}
				else
				{
					Neighbour = DataSet[NeighbourGrid.Position];
				}

				float Distance = FVector::Dist(CurrentGrid.Position, NeighbourGrid.Position);
				float TimeToReach = Current.TimeToReach + Distance / Speed;

				if (!NeighbourGrid.Invalid)
				{
					if (const float TentativeScore = Current.GScore + 100; TentativeScore < Neighbour.GScore)
					{
						//DrawDebugPoint(GetWorld(), NeighbourGrid.Position, 4.f, FColor::Cyan, true, -1.f, 0);
						Neighbour.CameFrom = Current.Coord;
						Neighbour.GScore = TentativeScore;

						//GScore + Heuristic Function
						Neighbour.FScore = Neighbour.GScore + (EndGrid.Position - NeighbourGrid.Position).SquaredLength();
						Neighbour.TimeToReach = TimeToReach;
						DataSet.Add(NeighbourGrid.Position, Neighbour);
						if (!bNeighbourPassed)
						{
							OpenSet.Add(Neighbour);
							HeapModify(OpenSet, OpenSet.Num()-1);
						}
					}
				}
				else
				{
					//DrawDebugPoint(GetWorld(), NeighbourGrid.Position, 4.f, FColor::Red, true, -1.f, 0);
				}
			}
		}
	}
	
	Status = EPathFindingStatus::Invalid;
	return Status;
}

void UAIPathFinder::StartMoving()
{
	GetWorld()->GetTimerManager().ClearTimer(MoveTimer);
	GetWorld()->GetTimerManager().SetTimer(MoveTimer,this, &UAIPathFinder::MoveAIAlongPath, UpdateFreq, true, UpdateFreq);
}

void UAIPathFinder::MoveAIAlongPath()
{
	Status = EPathFindingStatus::InProgress;
	for(int i = TotalPaths.Num() -1; i >= 0; i--)
	{
		DrawDebugPath();
		GetOwner()->SetActorLocation(TotalPaths[i].Position);
		/*const float Length = FVector::Dist(GetOwner()->GetActorLocation(),TotalPaths[i].Position);
		float L = 0;
		while (L < Length)
		{
			DrawDebugPath();
			FVector ForwardDirection = TotalPaths[i].Position - GetOwner()->GetActorLocation();
			ForwardDirection.Normalize();

			GetOwner()->SetActorLocation(TotalPaths[i].Position);
			
			L += GetWorld()->GetDeltaSeconds()*Speed;
		}*/
	}
	Status = EPathFindingStatus::Finished;
}

void UAIPathFinder::DrawDebugPath()
{
	if(TotalPaths.Num() > 0)
	{
		for(int i = TotalPaths.Num() - 2; i >= 0; i--)
		{
			DrawDebugLine(GetWorld(), TotalPaths[i+1].Position, TotalPaths[i].Position, FColor::Magenta, false, 10.f, 0.f);
		}
	}
}
