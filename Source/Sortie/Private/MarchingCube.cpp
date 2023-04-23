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
	MakeGridWithNoise();
	March();
}

void AMarchingCube::MakeGridWithNoise()
{
	//check chunk location for sampling the noise
	const FVector MapLoc = GetActorLocation();
	
	//add the grids (this looks ugly a little because unreal doesn't have multidimensional array!)  
	for(int x = 0; x < ChunkSize; x++)
	{
		FGridArray2D GridY;
		for(int y = 0; y < ChunkSize; y++)
		{
			FGridArray1D GridZ;
			for(int z = 0; z < ChunkHeight; z++)
			{
				//Perlin Noise 3D
				const float SampleX = (x + MapLoc.X/Scale) / NoiseScale;
				const float SampleY = (y + MapLoc.Y/Scale) / NoiseScale;
				const float SampleZ = (z + MapLoc.Z/Scale) / NoiseScale;

				const float PerlinValue = FMath::PerlinNoise3D(FVector(SampleX, SampleY, SampleZ));

				UE_LOG(LogTemp, Warning, TEXT("Perlin Value: %f %f %f --> %f"), SampleX, SampleY, SampleZ, PerlinValue);
				GridZ.Grids.Add(FGridPoint(FVector(x*Scale, y*Scale, z*Scale), PerlinValue >= NoiseThreshold));
				DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, z*Scale), 10.0f, 16, PerlinValue >= NoiseThreshold ? FColor::Green : FColor::Red, true, -1.0f, 0u, 0.0f);
			}
			GridY.Grids.Add(GridZ);
		}
		GridPoints.Grids.Add(GridY);
	}
}

// Marching the cube, full speed ahead!
void AMarchingCube::March()
{
	
}

