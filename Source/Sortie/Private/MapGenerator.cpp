// Fill out your copyright notice in the Description page of Project Settings.


#include "MapGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RandomStream.h"
#include "ProceduralMeshComponent.h"
#include "SortieCharacterBase.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	SetRootComponent(ProceduralMesh);
}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	Viewer = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass()));
	CreateProceduralTerrainChunk();
	
	//SetVisible(false); //On Initialize, let's hide it first, then we'll use update to check the position of the player
	//CheckVisible();
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckVisible();
}

void AMapGenerator::CreateProceduralTerrainChunk()
{
	CreateVertices();
	CreateTriangles();
	
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProceduralMesh->SetMaterial(0, Material);
}

void AMapGenerator::CheckVisible()
{
	const FVector ViewerLoc = Viewer->GetActorLocation();
	FVector MapBoundExtent;
	FVector MapBoundOrigin;
	GetActorBounds(false, MapBoundOrigin, MapBoundExtent);
	const float ViewerDistanceFromNearestEdge = FMath::Min(FVector::Dist2D(ViewerLoc, MapBoundOrigin), FVector::Dist2D(ViewerLoc, MapBoundExtent));
	SetVisible(ViewerDistanceFromNearestEdge <= Viewer->MaxViewDistance * Scale); //need to * scale to make the view and mapGen have the same scale
}

void AMapGenerator::SetVisible(bool Visible)
{
	SetActorHiddenInGame(!Visible);
}

//Function for creating a random perlin noise map
void AMapGenerator::CreateVertices()
{
	const FVector MapLoc = GetActorLocation();
	const FRandomStream* RandomStream = new FRandomStream(Seed);
	TArray<FVector2D> OctaveOffsets;

	for(int i = 0; i < Octaves; i++)
	{
		const float OffsetX = RandomStream->RandRange(0, 100);
		const float OffsetY = RandomStream->RandRange(0, 100);
		OctaveOffsets.Add(FVector2D(OffsetX, OffsetY));
	}
	
	for(int x=0; x < ChunkSize; x++)
	{
		for(int y=0; y < ChunkSize; y++)
		{
			//we're going to use this for sampling the octaves together
			float Amplitude = 1.0f;
			float Frequency = 1.0f;
			float NoiseHeight = 0.0f;

			//loop through all octaves and sample it to correct height
			for(int i = 0; i < Octaves ; i++)
			{
				const float SampleX = (x + MapLoc.X/Scale)/ NoiseScale * Frequency + OctaveOffsets[i].X * Frequency;
				const float SampleY = (y + MapLoc.Y/Scale)/ NoiseScale * Frequency + OctaveOffsets[i].Y * Frequency;

				const float PerlinValue = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY)); // * 2 - 1 so the value of the perlin is not negative
				NoiseHeight += PerlinValue * Amplitude;

				//UE_LOG(LogTemp, Warning, TEXT("height perlin: %f %f"), NoiseHeight * ZMultiplier, PerlinValue);

				Amplitude *= Persistance;
				Frequency *= Lacunarity;
				
			}

			const float z = NoiseHeight * ZMultiplier >= ZMultiplier / FlatLandThreshold ?
				NoiseHeight * ZMultiplier : FMath::Pow(2, ZMultiplier*(NoiseHeight - 1));
			
			//create vertices (Perlin noise)
			Vertices.Add(FVector(x * Scale, y * Scale, z));
			UV0.Add(FVector2D(x * UVScale, y * UVScale));

			//Debug the vertices spawning (Performance consuming, watch out!)
			//DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, NoiseHeight * ZMultiplier), 25.0f, 16, FColor::Red, true, -1.0f, 0u, 0.0f);
		}
	}
}

void AMapGenerator::CreateTriangles()
{
	//Credit: Sebastian Lague!
	const int MeshSimplificationIncrement = LOD == 0 ? 1 : LOD * 2;
	const int VerticesPerLine = (ChunkSize - 1) / MeshSimplificationIncrement + 1;
	const int TriangleParallelPoint = ChunkSize * MeshSimplificationIncrement;
	
	int VertexIndex = 0; //the current anchor point for drawing the triangle
	int VertexLine = 0; //the current line for vertexIndex

	for(int x = 0; x<ChunkSize; x+=MeshSimplificationIncrement)
	{
		for(int y=0; y< ChunkSize; y+=MeshSimplificationIncrement)
		{
			//Create a rectangle from 2 triangles (CCW)
			if(x <ChunkSize -1 && y <ChunkSize -1)
			{
				Triangles.Add(VertexIndex); //top left first triangle
				Triangles.Add(VertexIndex + TriangleParallelPoint + MeshSimplificationIncrement); // bottom right first triangle
				Triangles.Add(VertexIndex + TriangleParallelPoint); // bottom left first triangle
					
				Triangles.Add(VertexIndex + TriangleParallelPoint + MeshSimplificationIncrement); //bottom right second triangle
				Triangles.Add(VertexIndex); //top left second triangle
				Triangles.Add(VertexIndex + MeshSimplificationIncrement); // top right second triangle
			}
			VertexIndex+=MeshSimplificationIncrement;
		}
		VertexIndex = TriangleParallelPoint * (VertexLine+1);
		VertexLine++;
	}
}

