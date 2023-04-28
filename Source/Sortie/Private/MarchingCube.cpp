// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingCube.h"
#include "Constant/MarchingConst.h"
#include "Kismet/GameplayStatics.h"
#include "RealtimeMeshLibrary.h"
#include "SortieCharacterBase.h"

//#region Helper Classes
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

	//ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	//SetRootComponent(ProcMesh);

	RealtimeMesh = CreateDefaultSubobject<URealtimeMeshComponent>("RealtimeMesh");
	SetRootComponent(RealtimeMesh);
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

	//TODO: change to LOD of the mesh
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

float AMarchingCube::SmoothStep(const float MinValue, const float MaxValue, const float Dist)
{
	//interpolation function = -2x^3 + 3x^2
	const float DistMu = (Dist - MinValue) / (MaxValue - MinValue);
	return DistMu * (Dist / MinValue * 100);
}

void AMarchingCube::CreateVertex(const FGridPoint& CornerGridA, const FGridPoint& CornerGridB, const FVector& MapLoc)
{
	//Let's make it normal for now the mid point between the corner
	const FVector VertexPos = InterpolateEdgePosition(CornerGridA, CornerGridB);
	Vertices.Add(VertexPos - MapLoc);
	UV0.Add(FVector2D(VertexPos.X * UVScale / Scale, VertexPos.Y * UVScale / Scale));
	Normals.Add(FVector(0.f, 0.f, 1.f));
}

void AMarchingCube::CreateMesh()
{
	/*ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Material);*/

	//Runtime Mesh Creation
	FRealtimeMeshSimpleMeshData MeshData;
	MeshData.Positions = Vertices;
	MeshData.Triangles = Triangles;
	MeshData.UV0 = UV0;
	MeshData.Normals = Normals;
	
	Mesh = RealtimeMesh->InitializeRealtimeMesh<URealtimeMeshSimple>();
	Mesh->SetupMaterialSlot(0, "Primary Material", Material);
	MeshSection = Mesh->CreateMeshSection(0, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0), MeshData, true);
	
	CleanUpData();
}

void AMarchingCube::UpdateMesh()
{
	/*ProcMesh->ClearMeshSection(0);
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Material);*/
	
	//Runtime Mesh Creation
	FRealtimeMeshSimpleMeshData MeshData;
	MeshData.Positions = Vertices;
	MeshData.Triangles = Triangles;
	MeshData.UV0 = UV0;
	MeshData.Normals = Normals;

	Mesh->RemoveSection(MeshSection);
	MeshSection = Mesh->CreateMeshSection(0, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0), MeshData, true);
	CleanUpData();
}

void AMarchingCube::CleanUpData()
{
	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();
	Normals.Empty();
}

void AMarchingCube::CreateProceduralMarchingCubesChunk()
{
	//check chunk location for sampling the noise
	const FVector MapLoc = GetActorLocation();
	
	MakeGridWithNoise(MapLoc);
	March(MapLoc);
	
	//Finished Marching, Let's create a mesh from it
	if(Vertices.Num() > 0) CreateMesh();
}

void AMarchingCube::MakeGridWithNoise(const FVector& MapLoc)
{
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
				GridZ.Grids.Add({FVector(x*Scale + MapLoc.X, y*Scale + MapLoc.Y, z*Scale + MapLoc.Z), PerlinValue, PerlinValue >= NoiseThreshold});
				//DrawDebugBox(GetWorld(), FVector(x * Scale + MapLoc.X, y * Scale + MapLoc.Y, z*Scale + MapLoc.Z), FVector(3, 3, 3),FColor::Green,true,-1,0,0);
			}
			GridY.Grids.Add(GridZ);
		}
		GridPoints.Grids.Add(GridY);
	}
}

// Marching the cube, full speed ahead!
void AMarchingCube::March(const FVector& MapLoc)
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
					CreateVertex(CurrentCube.Points[A0], CurrentCube.Points[A1], MapLoc);
					CreateVertex(CurrentCube.Points[B0], CurrentCube.Points[B1], MapLoc);
					CreateVertex(CurrentCube.Points[C0], CurrentCube.Points[C1], MapLoc);

					Triangles.Add(Vertices.Num() - 3);
					Triangles.Add(Vertices.Num() - 2);
					Triangles.Add(Vertices.Num() - 1);
				}
			}
		}
	}
}

void AMarchingCube::Terraform(const FVector& HitLoc, const float SphereRadius, const float BrushForce)
{
	for(int x = 0; x< ChunkSize; x++)
	{
		for(int y = 0; y<ChunkSize; y++)
		{
			for(int z = 0; z<ChunkHeight; z++)
			{
				auto& [Position, Value, On] = GridPoints.Grids[x].Grids[y].Grids[z];
				//the grid is within the change radius, let's update its value
				if(const float GridDist = FVector::Dist(HitLoc, Position); GridDist < SphereRadius)
				{
					const float TerrainFormValue = SmoothStep(SphereRadius, SphereRadius * 0.5, GridDist) * NoiseThreshold * BrushForce / NoiseScale;
					Value += TerrainFormValue;
					On = Value >= NoiseThreshold;
				}
			}
		}
	}
	
	March(GetActorLocation());

	if (Vertices.Num() > 0) UpdateMesh();
}

