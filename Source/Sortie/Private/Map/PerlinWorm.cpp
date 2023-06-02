// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/PerlinWorm.h"

PerlinWorm::PerlinWorm()
{
	Duration = 0;
	Radius = 0;
	WormPitch = 0.f;
	WormRoll = 0.f;
	WormYaw = 0.f;
}

PerlinWorm::PerlinWorm(const int Dur, const int Rad, const float P, const float Y, const float R)
{
	Duration = Dur;
	Radius = Rad;
	WormPitch = P;
	WormRoll = R;
	WormYaw = Y;
}

PerlinWorm::~PerlinWorm()
{
}

void PerlinWorm::Wormify(AMCChunk* Chunk, AEndlessMap* ChunkManager, const FVector& Loc, int Time)
{
	if(!ChunkToUpdate.Contains(Chunk)) ChunkToUpdate.Add(Chunk);
	const FVector MapLoc = Chunk->GetActorLocation();
	RadialAddGrids(Chunk, FMath::PerlinNoise3D(Loc/Chunk->Scale)*Radius/2 + Radius, Loc);

	if (Time < Duration)
	{
		WormRoll = WormRoll + FMath::PerlinNoise3D(FVector(
		Loc.X+MapLoc.X,
		Loc.Y+MapLoc.Y,
		Loc.Z+MapLoc.Z*180
		));
		WormPitch = WormPitch + FMath::PerlinNoise3D(FVector(
		Loc.Z+MapLoc.Z,
		Loc.X+MapLoc.X,
		Loc.Y+MapLoc.Y*180
		));
		WormYaw = WormYaw + FMath::PerlinNoise3D(FVector(
		Loc.Y+MapLoc.Y,
		Loc.X+MapLoc.X,
		Loc.Z+MapLoc.Z*180
		));

		InternalForward = FVector(WormRoll, WormPitch, WormYaw);
		InternalForward.Normalize();

		FVector CurrentCoord = Chunk->ChunkCoord;
		const FVector NewPos = Loc + InternalForward*Chunk->Scale;
		DrawDebugLine(Chunk->GetWorld(),Loc, NewPos, FColor::Orange, true, -1.f, 0.f, 3.f);

		//Check if it's still in the same chunk or not
		CurrentCoord.X = NewPos.X < MapLoc.X ? CurrentCoord.X - 1 : NewPos.X <= MapLoc.X + Chunk->Scale * Chunk->ChunkSize ? CurrentCoord.X : CurrentCoord.X + 1;
		CurrentCoord.Y = NewPos.Y < MapLoc.Y ? CurrentCoord.Y - 1 : NewPos.Y <= MapLoc.Y + Chunk->Scale * Chunk->ChunkSize ? CurrentCoord.Y : CurrentCoord.Y + 1;
		CurrentCoord.Z = NewPos.Z < MapLoc.Z ? CurrentCoord.Z - 1 : NewPos.Z <= MapLoc.Z + Chunk->Scale * Chunk->ChunkHeight ? CurrentCoord.Z : CurrentCoord.Z + 1;
		
		
		if(ChunkManager->MapChunkDict.Contains(CurrentCoord))
		{
			if(CurrentCoord == Chunk->ChunkCoord)
			{
				Wormify(Chunk, ChunkManager, NewPos, Time + 1);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Worm Slip to other Chunk"));
				AMCChunk* CurrentChunk = Cast<AMCChunk>(ChunkManager->MapChunkDict[CurrentCoord]);
				Wormify(CurrentChunk, ChunkManager, NewPos, Time + 1);
			}
		}
	}
}

void PerlinWorm::RadialAddGrids(AMCChunk* Chunk, int Range, const FVector& Origin) const
{
	const FVector ChunkLoc = Chunk->GetActorLocation();

	const float RangeF = Range / Chunk->Scale;
	const FVector OriginToGridCal = FVector(
		FMath::CeilToInt(FMath::Abs(Origin.X - ChunkLoc.X)/Chunk->Scale),
		FMath::CeilToInt(FMath::Abs(Origin.Y - ChunkLoc.Y)/Chunk->Scale),
		FMath::FloorToInt(FMath::Abs(Origin.Z - ChunkLoc.Z)/Chunk->Scale)
		);

	
	//make sure this loop doesn't go out of calculation bound
	const int MinX = OriginToGridCal.X - 2*RangeF >= 0 ? static_cast<int>(OriginToGridCal.X - 2 * RangeF) : 0;
	const int MaxX = OriginToGridCal.X + 2*RangeF < Chunk->ChunkSize ? static_cast<int>(OriginToGridCal.X + 2 * RangeF) : Chunk->ChunkSize - 1;

	const int MinY = OriginToGridCal.Y - 2*RangeF >= 0 ? static_cast<int>(OriginToGridCal.Y - 2 * RangeF) : 0;
	const int MaxY = OriginToGridCal.Y + 2*RangeF < Chunk->ChunkSize ? static_cast<int>(OriginToGridCal.Y + 2 * RangeF) : Chunk->ChunkSize - 1;

	const int MinZ = OriginToGridCal.Z - 2*RangeF >= 0 ? static_cast<int>(OriginToGridCal.Z - 2 * RangeF) : 0;
	const int MaxZ = OriginToGridCal.Z + 2*RangeF < Chunk->ChunkHeight ? static_cast<int>(OriginToGridCal.Z + 2 * RangeF) : Chunk->ChunkHeight - 1;

	for(int x = MinX; x <= MaxX; ++x)
	{
		for(int y = MinY; y <= MaxY; ++y)
		{
			for(int z = MinZ; z <= MaxZ; ++z)
			{
				auto& [Position, Value, On] = Chunk->GridPoints.Grids[x].Grids[y].Grids[z];
				if(const float GridDist = FVector::Dist(Origin, Position); GridDist <= Range)
				{
					//DrawDebugPoint(Chunk->GetWorld(), Position, 1.f, On ? FColor::Green : FColor::Red,true, -1.f, 0);
					if (!On)
					{
						const float TerrainFormValue = Chunk->SmoothStep(Range, Range * 0.5, GridDist);
						Value += TerrainFormValue; 
						On = Value >= Chunk->NoiseThreshold;
					}
				}
			}
		}
	}
}


