// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/SPlayerCameraManager.h"

#include "SortieCharacterBase.h"
#include "Camera/CameraModifier.h"

DECLARE_CYCLE_STAT(TEXT("Camera ProcessViewRotation"), STAT_Camera_ProcessViewRotation, STATGROUP_Game);

void ASPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	SCOPE_CYCLE_COUNTER(STAT_Camera_ProcessViewRotation);

	const FRotator OldViewRotation = OutViewRotation;

	// Apply view modifications from active camera modifiers
	for (int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ++ModifierIdx)
	{
		if (ModifierList[ModifierIdx] != nullptr && !ModifierList[ModifierIdx]->IsDisabled())
		{
			if (ModifierList[ModifierIdx]->ProcessViewRotation(ViewTarget.Target, DeltaTime, OutViewRotation, OutDeltaRot))
			{
				break;
			}
		}
	}

	const ASortieCharacterBase* SortieChar = Cast<ASortieCharacterBase>(GetViewTargetPawn());
	const FVector ViewPlaneZ = SortieChar->GetActorAxisZ();

	if(!OutDeltaRot.IsZero())
	{
		//Obtain current view orthonormal axes
		FVector ViewRotationX, ViewRotationY, ViewRotationZ;
		FRotationMatrix(OutViewRotation).GetUnitAxes(ViewRotationX, ViewRotationY, ViewRotationZ);

		if (!ViewPlaneZ.IsZero())
		{
			// Yaw rotation should happen taking into account a determined plane to avoid weird orbits
			ViewRotationZ = ViewPlaneZ;
		}

		// Add delta rotation
		FQuat ViewRotation = OutViewRotation.Quaternion();
		if (OutDeltaRot.Pitch != 0.0f)
		{
			ViewRotation = FQuat(ViewRotationY, FMath::DegreesToRadians(-OutDeltaRot.Pitch)) * ViewRotation;
		}
		if (OutDeltaRot.Yaw != 0.0f)
		{
			ViewRotation = FQuat(ViewRotationZ, FMath::DegreesToRadians(OutDeltaRot.Yaw)) * ViewRotation;
		}
		if (OutDeltaRot.Roll != 0.0f)
		{
			ViewRotation = FQuat(ViewRotationX, FMath::DegreesToRadians(OutDeltaRot.Roll)) * ViewRotation;
		}
		
		OutViewRotation = ViewRotation.Rotator();

		// Consume delta rotation
		OutDeltaRot = FRotator::ZeroRotator;

		//The gravity change, so do the axis
		if (OutViewRotation != OldViewRotation)
		{
			if (!ViewPlaneZ.IsZero())
			{
				// Obtain current view orthonormal axes
				FVector ViewRotX, ViewRotY, ViewRotZ;
				FRotationMatrix(OutViewRotation).GetUnitAxes(ViewRotX, ViewRotY, ViewRotZ);

				// Obtain angle (with sign) between current view Z vector and plane normal
				float PitchAngle = FMath::RadiansToDegrees(FMath::Acos(ViewRotZ | ViewPlaneZ));
				if ((ViewRotX | ViewPlaneZ) < 0.0f)
				{
					PitchAngle *= -1.0f;
				}

				if (PitchAngle > ViewPitchMax)
				{
					// Make quaternion from zero pitch
					FQuat ViewRot(FRotationMatrix::MakeFromZY(ViewPlaneZ, ViewRotY));

					// Rotate 'up' with maximum pitch
					ViewRot = FQuat(ViewRotY, FMath::DegreesToRadians(-ViewPitchMax)) * ViewRot;

					OutViewRotation = ViewRot.Rotator();
				}
				else if (PitchAngle < ViewPitchMin)
				{
					// Make quaternion from zero pitch
					FQuat ViewRot(FRotationMatrix::MakeFromZY(ViewPlaneZ, ViewRotY));

					// Rotate 'down' with minimum pitch
					ViewRot = FQuat(ViewRotY, FMath::DegreesToRadians(-ViewPitchMin)) * ViewRot;

					OutViewRotation = ViewRot.Rotator();
				}
			}
			else
			{
				// Limit player view axes
				LimitViewPitch(OutViewRotation, ViewPitchMin, ViewPitchMax);
				LimitViewYaw(OutViewRotation, ViewYawMin, ViewYawMax);
				LimitViewRoll(OutViewRotation, ViewRollMin, ViewRollMax);
			}
		}
	}
}
