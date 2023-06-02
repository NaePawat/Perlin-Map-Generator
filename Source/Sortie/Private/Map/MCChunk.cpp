// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MCChunk.h"
#include "AI/AIManager.h"
#include "Async/Async.h"
#include "Constant/MarchingConst.h"
#include "Kismet/GameplayStatics.h"
#include "Map/EndlessMap.h"
#include "Map/PerlinWorm.h"
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
AMCChunk::AMCChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RealtimeMesh = CreateDefaultSubobject<URealtimeMeshComponent>("RealtimeMesh");
	SetRootComponent(RealtimeMesh);
}

// Called when the game starts or when spawned
void AMCChunk::BeginPlay()
{
	Super::BeginPlay();
	Viewer = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass()));
	AIManager = Cast<AAIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAIManager::StaticClass()));
	EndlessMap = Cast<AEndlessMap>(UGameplayStatics::GetActorOfClass(GetWorld(), AEndlessMap::StaticClass()));

	ChunkCoord = FVector(
		FMath::RoundToInt(GetActorLocation().X / (ChunkSize*Scale)),
		FMath::RoundToInt(GetActorLocation().Y / ChunkSize*Scale),
		FMath::RoundToInt(GetActorLocation().Z / (ChunkHeight*Scale))
	);

	CreateProceduralMarchingCubesChunk();
	GetWorld()->GetTimerManager().SetTimer(ChunkTimerHandle, this, &AMCChunk::UpdateAINavigation, 1.f, false, 1.f);
}

// Called every frame
void AMCChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: change to LOD of the mesh
}

FVector AMCChunk::InterpolateEdgePosition(const FGridPoint& CornerIndexA, const FGridPoint& CornerIndexB) const
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

float AMCChunk::SmoothStep(const float MinValue, const float MaxValue, const float Dist)
{
	const float DistMu = (Dist - MinValue) / (MaxValue - MinValue);
	return DistMu * (Dist / MinValue * 100);
}

void AMCChunk::CreateVertex(const FGridPoint& CornerGridA, const FGridPoint& CornerGridB, const FVector& MapLoc)
{
	//Let's make it normal for now the mid point between the corner
	const FVector VertexPos = InterpolateEdgePosition(CornerGridA, CornerGridB);
	Vertices.Add(VertexPos - MapLoc);
	UV0.Add(FVector2D(VertexPos.X * UVScale / Scale, VertexPos.Y * UVScale / Scale));
	Normals.Add(FVector(0.f, 0.f, 1.f));
}

void AMCChunk::CreateProcMesh()
{
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

void AMCChunk::UpdateProcMesh()
{
	//Runtime Mesh Creation
	FRealtimeMeshSimpleMeshData MeshData;
	MeshData.Positions = Vertices;
	MeshData.Triangles = Triangles;
	MeshData.UV0 = UV0;
	MeshData.Normals = Normals;

	Mesh->RemoveSection(MeshSection);
	MeshSection = Mesh->CreateMeshSection(0, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0), MeshData, true);

	//AI Nav need to be updated
	UpdateAINavigation();
	CleanUpData();
}

void AMCChunk::UpdateAINavigation() const
{
	AIManager->AddToChunkUpdateQueue(this);
}

void AMCChunk::CleanUpData()
{
	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();
	Normals.Empty();
}

void AMCChunk::CreateProceduralMarchingCubesChunk()
{
	//check chunk location for sampling the noise
	const FVector MapLoc = GetActorLocation();

	if(ChunkType == EChunkType::PerlinNoise)
	{
		MakeGrid(MapLoc);
		March(MapLoc);
		if(Vertices.Num() > 0) CreateProcMesh();
	}
	else
	{
		MakeGrid(MapLoc, false);
		TArray<AMCChunk*> ChunksNeedToUpdate = WormifyChunk(MapLoc);
		for(AMCChunk* Chunk : ChunksNeedToUpdate)
		{
			Chunk->March(Chunk->GetActorLocation());
			Chunk->MeshSection.IsValid() ? Chunk->UpdateProcMesh() : Chunk->CreateProcMesh();
		}
	}
}

void AMCChunk::MakeGrid(const FVector& MapLoc, bool WithNoise)
{
	//Check Seeds before adding in
	const FRandomStream* RandomStream = new FRandomStream(Seed);
	TArray<FVector> OctaveOffsets;

	for(int i = 0; i< Octaves; i++)
	{
		const float OffsetX = RandomStream->RandRange(0,100);
		const float OffsetY = RandomStream->RandRange(0,100);
		const float OffsetZ = RandomStream->RandRange(0,100);
		OctaveOffsets.Add(FVector(OffsetX, OffsetY, OffsetZ));
	}
	
	//Add the grids (this looks ugly a little because unreal doesn't have multidimensional array!)  
	for(int x = 0; x < ChunkSize/LOD; x++)
	{
		FGridPointArray2D GridY;
		for(int y = 0; y < ChunkSize/LOD; y++)
		{
			FGridPointArray1D GridZ;
			for(int z = 0; z < ChunkHeight/LOD; z++)
			{
				if (WithNoise)
				{
					//we're going to use this for sampling the octaves together
					float Amplitude = 1.f;
					float Frequency = 1.f;
					float Noisiness = 0.f;

					for(int i = 0; i < Octaves; i++)
					{
						//Perlin Noise 3D
						const float SampleX = (x + MapLoc.X/Scale) / NoiseScale * Frequency + OctaveOffsets[i].X * Frequency;
						const float SampleY = (y + MapLoc.Y/Scale) / NoiseScale * Frequency + OctaveOffsets[i].Y * Frequency;
						const float SampleZ = (z + MapLoc.Z/Scale) / NoiseScale * Frequency + OctaveOffsets[i].Z * Frequency;

						const float PerlinValue = FMath::PerlinNoise3D(FVector(SampleX, SampleY, SampleZ));
						Noisiness += PerlinValue * Amplitude;
						Amplitude *= Persistance;
						Frequency *= Lacunarity;
					}
					
					GridZ.Grids.Add({FVector(x*Scale*LOD + MapLoc.X, y*Scale*LOD + MapLoc.Y, z*Scale*LOD + MapLoc.Z), Noisiness, Noisiness >= NoiseThreshold});
				}
				else
				{
					GridZ.Grids.Add({FVector(x*Scale*LOD + MapLoc.X, y*Scale*LOD + MapLoc.Y, z*Scale*LOD + MapLoc.Z), 0, false});
				}
			}
			GridY.Grids.Add(GridZ);
		}
		GridPoints.Grids.Add(GridY);
	}
}

// Marching the cube, full speed ahead!
void AMCChunk::March(const FVector& MapLoc)
{
	for(int x = 0; x < ChunkSize/LOD - 1; x++)
	{
		for(int y = 0; y < ChunkSize/LOD - 1; y++)
		{
			for(int z = 0; z < ChunkHeight/LOD - 1; z++)
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

void AMCChunk::Terraform(const FVector& HitLoc, const float SphereRadius, const float BrushForce)
{
	bool bIsEffected = false;
	const FVector ChunkLoc = GetActorLocation();

	const float RangeF = SphereRadius /Scale;
	const FVector HitLocToGridCal = FVector(
		FMath::CeilToInt(FMath::Abs(HitLoc.X - ChunkLoc.X)/Scale),
		FMath::CeilToInt(FMath::Abs(HitLoc.Y - ChunkLoc.Y)/Scale),
		FMath::FloorToInt(FMath::Abs(HitLoc.Z - ChunkLoc.Z)/Scale)
		);
	
	//make sure this loop doesn't go out of calculation bound
	const int MinX = HitLocToGridCal.X - 2*RangeF >= 0 ? static_cast<int>(HitLocToGridCal.X - 2 * RangeF) : 0;
	const int MaxX = HitLocToGridCal.X + 2*RangeF < ChunkSize ? static_cast<int>(HitLocToGridCal.X + 2 * RangeF) : ChunkSize - 1;

	const int MinY = HitLocToGridCal.Y - 2*RangeF >= 0 ? static_cast<int>(HitLocToGridCal.Y - 2 * RangeF) : 0;
	const int MaxY = HitLocToGridCal.Y + 2*RangeF < ChunkSize ? static_cast<int>(HitLocToGridCal.Y + 2 * RangeF) : ChunkSize - 1;

	const int MinZ = HitLocToGridCal.Z - 2*RangeF >= 0 ? static_cast<int>(HitLocToGridCal.Z - 2 * RangeF) : 0;
	const int MaxZ = HitLocToGridCal.Z + 2*RangeF < ChunkHeight ? static_cast<int>(HitLocToGridCal.Z + 2 * RangeF) : ChunkHeight - 1;
	
	for(int x = MinX; x <= MaxX; ++x)
	{
		for(int y = MinY; y <= MaxY; ++y)
		{
			for(int z = MinZ; z <= MaxZ; ++z)
			{
				auto& [Position, Value, On] = GridPoints.Grids[x].Grids[y].Grids[z];
				//the grid is within the change radius, let's update its value
				if(const float GridDist = FVector::Dist(HitLoc, Position); GridDist < SphereRadius)
				{
					bIsEffected = true;
					const float TerrainFormValue = SmoothStep(SphereRadius, SphereRadius * 0.5, GridDist) * NoiseThreshold * BrushForce / NoiseScale;
					Value += TerrainFormValue;
					On = Value >= NoiseThreshold;
				}
			}
		}
	}

	const TFuture<void> TerraformingTask = Async(EAsyncExecution::ThreadPool, [=]
	{
		if (bIsEffected) March(GetActorLocation());
		if (Vertices.Num() > 0) UpdateProcMesh();
	});

	TerraformingTask.Wait();
}

TArray<AMCChunk*> AMCChunk::WormifyChunk(const FVector& ChunkLoc)
{
	TArray<AMCChunk*> ChunkChange;
	const FVector CenterLoc = FVector(
		(ChunkLoc.X + ChunkSize*Scale) / 2,
		(ChunkLoc.Y + ChunkSize*Scale) / 2,
		(ChunkLoc.Z + ChunkHeight*Scale) / 2
	);
	const int NumWorms = FMath::RandRange(1, MaxWorms);
	for(int i=0; i < NumWorms; i++)
	{
		const float WormLength = FMath::RandRange(MinWormLength, MaxWormLength);
		const float WormRadius = FMath::RandRange(MinWormRadius, MaxWormRadius);

		PerlinWorm* Worm = new PerlinWorm(WormLength, WormRadius, FMath::Sin(WormLength), FMath::Cos(WormRadius), FMath::Tan(WormLength + WormRadius));
		Worm->Wormify(this, EndlessMap, CenterLoc);
		for(AMCChunk* Chunk : Worm->ChunkToUpdate)
		{
			ChunkChange.AddUnique(Chunk);
		}
	}
	return ChunkChange;
}

TArray<AMCChunk*> AMCChunk::GetNeighborChunks() const
{
	TArray<AMCChunk*> Neighbors;

	for(int x = -1; x<=1 ; ++x)
	{
		for(int y = -1; y<=1 ; ++y)
		{
			for(int z = -1; z<=1 ; ++z)
			{
				if (x == 0 && y == 0 && z == 0) continue;
				const FVector CurrentCoord = FVector(ChunkCoord.X + x, ChunkCoord.Y + y, ChunkCoord.Z + z);
				if(EndlessMap->MapChunkDict.Contains(CurrentCoord))
				{
					Neighbors.Add(Cast<AMCChunk>(EndlessMap->MapChunkDict[CurrentCoord]));
				}
			}
		}
	}

	return Neighbors;
}

