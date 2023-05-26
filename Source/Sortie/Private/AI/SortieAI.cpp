// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SortieAI.h"
#include "AI/AIPathFinder.h"
#include "Kismet/GameplayStatics.h"
#include "SortieCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

bool FPathFindingTask::Init()
{
	StopTaskCounter.Reset();
	bIsThreadRunning = true;
	return true;
}

uint32 FPathFindingTask::Run()
{
	while (!StopTaskCounter.GetValue() && bIsThreadRunning)
	{
		if(!StartEndQueue.IsEmpty())
		{
			FStartEnd StartEnd;
			StartEndQueue.Dequeue(StartEnd);
			Finder->PathFinding(StartEnd.Start, StartEnd.End);
		}
	}
	return 0;
}

void FPathFindingTask::Stop()
{
	StopTaskCounter.Increment();
	bIsThreadRunning = false;
}

void FPathFindingTask::AddTask(const FVector& Start, const FVector& End)
{
	StartEndQueue.Enqueue({Start, End});
}

// Sets default values
ASortieAI::ASortieAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASortieAI::BeginPlay()
{
	Super::BeginPlay();

	Agent = FindComponentByClass<UAIPathFinder>();
	if(Agent)
	{
		if (ASortieCharacterBase* Player = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass())))
		{
			Target = Player;
			AIThread = new FPathFindingTask(Agent);
			CurrentRunningThread = FRunnableThread::Create(AIThread, TEXT("Natty AI Thread"));
			GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ASortieAI::MoveFromAToB, Interval, true, Interval);
		}
	}
}

void ASortieAI::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(CurrentRunningThread && AIThread->IsRunning())
	{
		CurrentRunningThread->Suspend(true);
		AIThread->StopRunning();
		CurrentRunningThread->Suspend(false);
		CurrentRunningThread->Kill(false);
		CurrentRunningThread->WaitForCompletion();

		AIThread = nullptr;
		delete CurrentRunningThread;
	}
}

void ASortieAI::MoveFromAToB()
{
	if(PrevTargetLoc != Target->GetActorLocation() && !Target->GetCharacterMovement()->IsFalling())
	{
		PrevTargetLoc = Target->GetActorLocation();
		AIThread->AddTask(GetActorLocation(),PrevTargetLoc);
	}
}

void ASortieAI::MoveAIAlongPath(float DeltaTime)
{
	const FVector CurrentLoc  = GetActorLocation();
	const FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, CurrentTargetGrid, DeltaTime, Speed);

	SetActorLocation(NewLoc);

	if(FVector::DistSquared(CurrentLoc, CurrentTargetGrid) < MaxTargetGridOffset)
	{
		TargetGridIndex--;
		if(TargetGridIndex <= 0)
		{
			TargetGridIndex = 0;
			Agent->Status = EPathFindingStatus::Finished;
		}
	}

	Agent->Status = EPathFindingStatus::InProgress;
	CurrentTargetGrid = TotalPaths[TargetGridIndex].Position;
}

// Called every frame
void ASortieAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(TotalPaths.Num() > 0) MoveAIAlongPath(DeltaTime);
}

// Called to bind functionality to input
void ASortieAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

