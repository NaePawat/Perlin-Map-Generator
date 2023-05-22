// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SortieAI.h"
#include "AI/AIPathFinder.h"
#include "Kismet/GameplayStatics.h"
#include "SortieCharacterBase.h"

// Sets default values
ASortieAI::ASortieAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASortieAI::BeginPlay()
{
	Super::BeginPlay();

	Agent = FindComponentByClass<UAIPathFinder>();
	if(Agent)
	{
		PointA = GetActorLocation();

		if (const ASortieCharacterBase* Player = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass())))
		{
			PointA = GetActorLocation();
			PointB = Player->GetActorLocation();
			GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ASortieAI::MoveFromAToB, Interval, true, Interval);
		}
	}
}

void ASortieAI::MoveFromAToB()
{
	/*while (true)
	{
		Agent->PathFinding(PointB);
		while (Agent->Status == EPathFindingStatus::Invalid)
		{
			const FVector POM = PointA;
			PointA = PointB;
			PointB = POM;
			Agent->PathFinding(PointB);
		}
		while (Agent->Status != EPathFindingStatus::Finished)
		{
			return;
		}
		const FVector POM = PointA;
		PointA = PointB;
		PointB = POM;
		return;
	}*/
	Agent->PathFinding(PointB);
	UE_LOG(LogTemp, Warning, TEXT("Status: %d"), Agent->Status);
}

// Called every frame
void ASortieAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASortieAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

