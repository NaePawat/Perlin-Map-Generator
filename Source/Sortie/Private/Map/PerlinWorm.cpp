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

void PerlinWorm::Wormify(AMCChunk* Chunk, const FVector& Loc, int Time)
{
	RadialAddGrids(Chunk, FMath::PerlinNoise3D(Loc/Chunk->Scale)*Radius/2 + Radius, Loc);
}

void PerlinWorm::RadialAddGrids(AMCChunk* Chunk, int Range, const FVector& Origin) const
{
	const float RangeF = Range / Chunk->Scale;
	const FVector ChunkLoc = Chunk->GetActorLocation();
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
				if(!On && FVector::DistSquared(Origin, Position) <= Range) On = true;
			}
		}
	}
}


