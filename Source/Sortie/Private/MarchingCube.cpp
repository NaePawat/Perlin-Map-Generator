// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCube.h"
#include "Constant/MarchingConst.h"
#include "Kismet/GameplayStatics.h"
#include "ProceduralMeshComponent.h"
#include "SortieCharacterBase.h"

//#region Helper Classes
/**
 * Cube class is a combination of Grid class at 8 points to create a cube, and record the cube configuration
 */
FCube::FCube()
{
	Points.Add({FVector(0,0,0), 0.f, false});
	Points.Add({FVector(1,0,0), 0.f, false});
	Points.Add({FVector(1,1,0), 0.f,false});
	Points.Add({FVector(0,1,0), 0.f, false});
	Points.Add({FVector(0,0,1), 0.f, false});
	Points.Add({FVector(1,0,1), 0.f, false});
	Points.Add({FVector(1,1,1), 0.f, false});
	Points.Add({FVector(0,1,1), 0.f, false});
	Config = 0;
}

FCube::FCube(const FGridPoint& PosZero, const FGridPoint& PosOne, const FGridPoint& PosTwo, const FGridPoint& PosThree,
	const FGridPoint& PosFour, const FGridPoint& PosFive, const FGridPoint& PosSix, const FGridPoint& PosSeven)
{
	Points.Add(PosZero);
	Points.Add(PosOne);
	Points.Add(PosTwo);
	Points.Add(PosThree);
	Points.Add(PosFour);
	Points.Add(PosFive);
	Points.Add(PosSix);
	Points.Add(PosSeven);

	Config = CalculateConfig();
}

FCube::~FCube()
{
}

int FCube::CalculateConfig()
{
	Config += Points[0].On ? 1 : 0;
	Config += Points[1].On ? 2 : 0;
	Config += Points[2].On ? 4 : 0;
	Config += Points[3].On ? 8 : 0;
	Config += Points[4].On ? 16 : 0;
	Config += Points[5].On ? 32 : 0;
	Config += Points[6].On ? 64 : 0;
	Config += Points[7].On ? 128 : 0;
	
	return Config;
}

//#endregion

// Sets default values
AMarchingCube::AMarchingCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	SetRootComponent(ProceduralMesh);
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

FVector AMarchingCube::InterpolateEdgePosition(const FGridPoint& CornerIndexA, const FGridPoint& CornerIndexB) const
{
	if(FMath::IsNearlyZero(NoiseThreshold - CornerIndexA.Value)) return CornerIndexA.Position;
	if(FMath::IsNearlyZero(NoiseThreshold - CornerIndexB.Value)) return CornerIndexB.Position;
	if(FMath::IsNearlyZero(CornerIndexA.Value - CornerIndexB.Value)) return CornerIndexA.Position;

	const float Mu = (NoiseThreshold - CornerIndexA.Value) / (CornerIndexB.Value - CornerIndexA.Value);

	return FVector(
		CornerIndexA.Position.X + Mu * (CornerIndexB.Position.X - CornerIndexA.Position.X),
		CornerIndexA.Position.Y + Mu * (CornerIndexB.Position.Y - CornerIndexA.Position.Y),
		CornerIndexA.Position.Z + Mu * (CornerIndexB.Position.Z - CornerIndexA.Position.Z)
	);
}

void AMarchingCube::CreateVertex(const FGridPoint& CornerGridA, const FGridPoint& CornerGridB)
{
	//Let's make it normal for now the mid point between the corner
	const FVector VertexPos = InterpolateEdgePosition(CornerGridA, CornerGridB);
	Vertices.Add(VertexPos);
	UV0.Add(FVector2D(VertexPos.X * UVScale / Scale, VertexPos.Y * UVScale / Scale));
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

				//UE_LOG(LogTemp, Warning, TEXT("Perlin Value: %f %f %f --> %f"), SampleX, SampleY, SampleZ, PerlinValue);
				GridZ.Grids.Add({FVector(x*Scale, y*Scale, z*Scale), PerlinValue, PerlinValue >= NoiseThreshold});
				//DrawDebugSphere(GetWorld(), FVector(x * Scale, y * Scale, z*Scale), 10.0f, 16, PerlinValue >= NoiseThreshold ? FColor::Green : FColor::Red, true, -1.0f, 0u, 0.0f);
			}
			GridY.Grids.Add(GridZ);
		}
		GridPoints.Grids.Add(GridY);
	}
}

// Marching the cube, full speed ahead!
void AMarchingCube::March()
{
	for(int x = 0; x < ChunkSize - 1; x++)
	{
		for(int y = 0; y < ChunkSize - 1; y++)
		{
			for(int z = 0; z < ChunkHeight - 1; z++)
			{
				//current cube corners
				const FCube CurrentCube = FCube(
					GridPoints.Grids[x].Grids[y].Grids[z],
					GridPoints.Grids[x+1].Grids[y].Grids[z],
					GridPoints.Grids[x+1].Grids[y+1].Grids[z],
					GridPoints.Grids[x].Grids[y+1].Grids[z],
					GridPoints.Grids[x].Grids[y].Grids[z+1],
					GridPoints.Grids[x+1].Grids[y].Grids[z+1],
					GridPoints.Grids[x+1].Grids[y+1].Grids[z+1],
					GridPoints.Grids[x].Grids[y+1].Grids[z+1]
				);
				
				//Get Configuration on the table
				const int* EdgesIndices = MarchingConst::Triangulation[CurrentCube.Config];

				for(int i = 0; i<16 ;i +=3)
				{
					//If the edge is -1, then there's no further vertices in this config
					if(EdgesIndices[i] == -1) break;

					//Get Indices of two corner points for that edge. Do this three times for triangle
					const int EdgeIndexA = EdgesIndices[i];
					const int A0 = MarchingConst::CornerIndexAFromEdge[EdgeIndexA];
					const int A1 = MarchingConst::CornerIndexBFromEdge[EdgeIndexA];
					
					const int EdgeIndexB = EdgesIndices[i+1];
					const int B0 = MarchingConst::CornerIndexAFromEdge[EdgeIndexB];
					const int B1 = MarchingConst::CornerIndexBFromEdge[EdgeIndexB];
					
					const int EdgeIndexC = EdgesIndices[i+2];
					const int C0 = MarchingConst::CornerIndexAFromEdge[EdgeIndexC];
					const int C1 = MarchingConst::CornerIndexBFromEdge[EdgeIndexC];

					//Calculate the position of each vertex
					CreateVertex(CurrentCube.Points[A0], CurrentCube.Points[A1]);
					CreateVertex(CurrentCube.Points[B0], CurrentCube.Points[B1]);
					CreateVertex(CurrentCube.Points[C0], CurrentCube.Points[C1]);

					Triangles.Add(Vertices.Num() - 3);
					Triangles.Add(Vertices.Num() - 2);
					Triangles.Add(Vertices.Num() - 1);
				}
			}
		}
	}

	/*UE_LOG(LogTemp, Warning, TEXT("------------------Vertices------------------"));
	for(FVector Ver:Vertices)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Ver.ToString());
		DrawDebugSphere(GetWorld(), Ver, 10.0f, 16, FColor::Blue, true, -1.0f, 0u, 0.0f);

	}
	UE_LOG(LogTemp, Warning, TEXT("------------------UV------------------"));
	for(FVector2D UV:UV0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *UV.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("------------------Triangles------------------"));
	for(int Tri:Triangles)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), Tri);
	}*/
	
	//Finished Marching, Let's create a mesh from it
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProceduralMesh->SetMaterial(0, Material);
}

