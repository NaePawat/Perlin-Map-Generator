// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SORTIE_API ASPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
};
