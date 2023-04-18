// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
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

void AMapGenerator::CreateVertices()
{
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
				const float SampleX = x / NoiseScale * Frequency;
				const float SampleY = y / NoiseScale * Frequency;

				const float PerlinValue = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY)); // * 2 - 1 so the value of the perlin is not negative
				NoiseHeight += PerlinValue * Amplitude;

				/*UE_LOG(LogTemp, Warning, TEXT("noise perlin: %f %f"), NoiseHeight, PerlinValue);*/

				Amplitude *= Persistance;
				Frequency *= Lacunarity;
			}
			
			//create random height for the vertices (Perlin noise)
			/*const float z = FMath::PerlinNoise2D(FVector2D(x * NoiseScale + 0.1, y * NoiseScale + 0.1)) * ZMultiplier;*/
			
			Vertices.Add(FVector(x * Scale, y * Scale, NoiseHeight * ZMultiplier));
			UV0.Add(FVector2D(x * UVScale, y * UVScale));

			//Debug the vertices spawning
			DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, 0), 25.0f, 16, FColor::Red, true, -1.0f, 0u, 0.0f);
		}
	}
}

void AMapGenerator::CreateTriangles()
{
	for(int x=0; x < XSize; x++)
	{
		for(int y=0; y < YSize; y++)
		{
			//Create a rectangle from 2 triangles (CCW)
			int BottomLeftVertices = y + YSize * x; 
			
			Triangles.Add(BottomLeftVertices); // bottom left first triangle
			Triangles.Add( BottomLeftVertices + 1); // bottom right first triangle
			Triangles.Add(YSize + BottomLeftVertices); //top left first triangle
			
			Triangles.Add(BottomLeftVertices +1); //bottom right second triangle
			Triangles.Add(YSize + BottomLeftVertices + 1); // top right second triangle
			Triangles.Add(YSize + BottomLeftVertices); //top left second triangle
		}
	}
}

