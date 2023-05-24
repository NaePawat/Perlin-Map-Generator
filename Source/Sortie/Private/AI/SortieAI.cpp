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
		if (ASortieCharacterBase* Player = Cast<ASortieCharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ASortieCharacterBase::StaticClass())))
		{
			Target = Player;
			GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &ASortieAI::MoveFromAToB, Interval, true, Interval);
		}
	}
}

void ASortieAI::MoveFromAToB()
{
	if(PrevTargetLoc != Target->GetActorLocation())
	{
		PrevTargetLoc = Target->GetActorLocation();
		Agent->PathFinding(PrevTargetLoc);
	}
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

