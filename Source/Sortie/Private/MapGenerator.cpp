// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
#include "Math/RandomStream.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	CreateVertices();
	CreateTriangles();
	
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	UE_LOG(LogTemp, Warning, TEXT("current total vertices: %d"), Vertices.Num());
	UE_LOG(LogTemp, Warning, TEXT("current total triangles: %d"), Triangles.Num());
	ProceduralMesh->SetMaterial(0, Material);
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Function for creating a random perlin noise map
void AMapGenerator::CreateVertices()
{
	const FRandomStream* RandomStream = new FRandomStream(Seed);
	TArray<FVector2D> OctaveOffsets;

	for(int i = 0; i < Octaves; i++)
	{
		const float OffsetX = RandomStream->RandRange(0, 100);
		const float OffsetY = RandomStream->RandRange(0, 100);
		OctaveOffsets.Add(FVector2D(OffsetX, OffsetY));
	}
	
	for(int x=0; x < XSize; x++)
	{
		for(int y=0; y < YSize; y++)
		{
			//we're going to use this for sampling the octaves together
			float Amplitude = 1.0f;
			float Frequency = 1.0f;
			float NoiseHeight = 0.0f;

			//loop through all octaves and sample it to correct height
			for(int i = 0; i < Octaves ; i++)
			{
				const float SampleX = x / NoiseScale * Frequency + OctaveOffsets[i].X;
				const float SampleY = y / NoiseScale * Frequency + OctaveOffsets[i].Y;

				const float PerlinValue = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY)); // * 2 - 1 so the value of the perlin is not negative
				NoiseHeight += PerlinValue * Amplitude;

				/*UE_LOG(LogTemp, Warning, TEXT("noise perlin: %f %f"), NoiseHeight, PerlinValue);*/

				Amplitude *= Persistance;
				Frequency *= Lacunarity;
			}
			
			//create vertices (Perlin noise)
			Vertices.Add(FVector(x * Scale, y * Scale, NoiseHeight * ZMultiplier));
			UV0.Add(FVector2D(x * UVScale, y * UVScale));

			//Debug the vertices spawning (Performance consuming, watch out!)
			//DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, NoiseHeight * ZMultiplier), 25.0f, 16, FColor::Red, true, -1.0f, 0u, 0.0f);
		}
	}
}

void AMapGenerator::CreateTriangles()
{
	//Credit: Sebastian Lague!
	int VertexIndex = 0;
	for(int x = 0; x < XSize; x++)
	{
		for(int y = 0; y< YSize; y++)
		{
			//Create a rectangle from 2 triangles (CCW)
			if(x < XSize-1 && y < YSize-1)
			{
				Triangles.Add(VertexIndex); //top left first triangle
				Triangles.Add(VertexIndex + YSize + 1); // bottom right first triangle
				Triangles.Add(VertexIndex + YSize); // bottom left first triangle
				
				Triangles.Add(VertexIndex + YSize + 1); //bottom right second triangle
				Triangles.Add(VertexIndex); //top left second triangle
				Triangles.Add(VertexIndex + 1); // top right second triangle
			}

			VertexIndex++;
		}
	}
}

