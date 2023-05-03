// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/SCharacterMovementComponent.h"
#include "Engine/ScopedMovementUpdate.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

constexpr float GVertical_Slope_Normal_Z = 0.001f;
constexpr float GMax_Step_Side_Z = 0.08f;

USCharacterMovementComponent::USCharacterMovementComponent()
{
	bFallingRemoveSpeedZ  = true;
	bIgnoreBaseRollMove = true;
	CustomGravityDirection = FVector::ZeroVector;
}

FVector USCharacterMovementComponent::CalcAnimRootMotionVelocity(const FVector& RootMotionDeltaMove, float DeltaSeconds,
	const FVector& CurrentVelocity) const
{
	return Super::CalcAnimRootMotionVelocity(RootMotionDeltaMove, DeltaSeconds, CurrentVelocity);
}

void USCharacterMovementComponent::StartFalling(int32 Iterations, float remainingTime, float timeTick,
	const FVector& Delta, const FVector& subLoc)
{
	Super::StartFalling(Iterations, remainingTime, timeTick, Delta, subLoc);
}

void USCharacterMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	Super::PhysFalling(deltaTime, Iterations);
}

FVector USCharacterMovementComponent::GetFallingLateralAcceleration(float DeltaTime)
{
	return Super::GetFallingLateralAcceleration(DeltaTime);
}

FVector USCharacterMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity,
	float DeltaTime) const
{
	return Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);
}

void USCharacterMovementComponent::UpdateBasedMovement(float DeltaSeconds)
{
	Super::UpdateBasedMovement(DeltaSeconds);
}

bool USCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	return Super::DoJump(bReplayingMoves);
}

FVector USCharacterMovementComponent::GetImpartedMovementBaseVelocity() const
{
	return Super::GetImpartedMovementBaseVelocity();
}

void USCharacterMovementComponent::JumpOff(AActor* MovementBaseActor)
{
	Super::JumpOff(MovementBaseActor);
}

void USCharacterMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult,
	bool bZeroDelta, const FHitResult* DownwardSweepResult) const
{
	Super::FindFloor(CapsuleLocation, OutFloorResult, bZeroDelta, DownwardSweepResult);
}

void USCharacterMovementComponent::UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation)
{
	Super::UpdateBasedRotation(FinalRotation, ReducedRotation);
}

bool USCharacterMovementComponent::FloorSweepTest(FHitResult& OutHit, const FVector& Start, const FVector& End,
	ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params,
	const FCollisionResponseParams& ResponseParam) const
{
	return Super::FloorSweepTest(OutHit, Start, End, TraceChannel, CollisionShape, Params, ResponseParam);
}

bool USCharacterMovementComponent::IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const
{
	return Super::IsValidLandingSpot(CapsuleLocation, Hit);
}

bool USCharacterMovementComponent::ShouldCheckForValidLandingSpot(float DeltaTime, const FVector& Delta,
	const FHitResult& Hit) const
{
	return Super::ShouldCheckForValidLandingSpot(DeltaTime, Delta, Hit);
}

bool USCharacterMovementComponent::ShouldComputePerchResult(const FHitResult& InHit, bool bCheckRadius) const
{
	return Super::ShouldComputePerchResult(InHit, bCheckRadius);
}

bool USCharacterMovementComponent::ComputePerchResult(const float TestRadius, const FHitResult& InHit,
	const float InMaxFloorDist, FFindFloorResult& OutPerchFloorResult) const
{
	return Super::ComputePerchResult(TestRadius, InHit, InMaxFloorDist, OutPerchFloorResult);
}

bool USCharacterMovementComponent::CanStepUp(const FHitResult& Hit) const
{
	return Super::CanStepUp(Hit);
}

bool USCharacterMovementComponent::StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult& Hit,
	FStepDownResult* OutStepDownResult)
{
	return Super::StepUp(GravDir, Delta, Hit, OutStepDownResult);
}

void USCharacterMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	Super::SetMovementMode(NewMovementMode, NewCustomMode);
}

void USCharacterMovementComponent::SetDefaultMovementMode()
{
	Super::SetDefaultMovementMode();
}

void USCharacterMovementComponent::AdjustFloorHeight()
{
	Super::AdjustFloorHeight();
}

bool USCharacterMovementComponent::CheckLedgeDirection(const FVector& OldLocation, const FVector& SideStep,
	const FVector& GravDir) const
{
	return Super::CheckLedgeDirection(OldLocation, SideStep, GravDir);
}

FVector USCharacterMovementComponent::GetLedgeMove(const FVector& OldLocation, const FVector& Delta,
	const FVector& GravDir) const
{
	return Super::GetLedgeMove(OldLocation, Delta, GravDir);
}

void USCharacterMovementComponent::StartNewPhysics(float deltaTime, int32 Iterations)
{
	Super::StartNewPhysics(deltaTime, Iterations);
}

void USCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

void USCharacterMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	Super::ApplyAccumulatedForces(DeltaSeconds);
}

bool USCharacterMovementComponent::IsWalkable(const FHitResult& Hit) const
{
	return Super::IsWalkable(Hit);
}

void USCharacterMovementComponent::ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance,
	float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius,
	const FHitResult* DownwardSweepResult) const
{
	Super::ComputeFloorDist(CapsuleLocation, LineDistance, SweepDistance, OutFloorResult, SweepRadius,
	                        DownwardSweepResult);
}

bool USCharacterMovementComponent::IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint,
	const float CapsuleRadius) const
{
	return Super::IsWithinEdgeTolerance(CapsuleLocation, TestImpactPoint, CapsuleRadius);
}

void USCharacterMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	Super::PhysFlying(deltaTime, Iterations);
}

float USCharacterMovementComponent::BoostAirControl(float DeltaTime, float TickAirControl,
	const FVector& FallAcceleration)
{
	return Super::BoostAirControl(DeltaTime, TickAirControl, FallAcceleration);
}

void USCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void USCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void USCharacterMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}

void USCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

void USCharacterMovementComponent::MaintainHorizontalGroundVelocity()
{
	Super::MaintainHorizontalGroundVelocity();
}

float USCharacterMovementComponent::SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal,
	FHitResult& Hit, bool bHandleImpact)
{
	return Super::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}

void USCharacterMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	Super::SetPostLandedPhysics(Hit);
}

void USCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);
}

FVector USCharacterMovementComponent::ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit,
	const bool bHitFromLineTrace) const
{
	return Super::ComputeGroundMovementDelta(Delta, RampHit, bHitFromLineTrace);
}

void USCharacterMovementComponent::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds,
	FStepDownResult* OutStepDownResult)
{
	Super::MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);
}

void USCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	Super::SimulateMovement(DeltaTime);
}

FVector USCharacterMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	return Super::ConstrainInputAcceleration(InputAcceleration);
}

FVector USCharacterMovementComponent::ScaleInputAcceleration(const FVector& InputAcceleration) const
{
	return Super::ScaleInputAcceleration(InputAcceleration);
}

FVector USCharacterMovementComponent::GetGravityDirection(bool bAvoidZeroGravity) const
{
	//Note that GravityZ and Gravity Scale is a float value 
	if(GravityScale != 0.f)
	{
		//if we have the custom gravity direction applied
		if (!CustomGravityDirection.IsZero())
		{
			return CustomGravityDirection * (GravityScale > 0.f ? 1.f : -1.f);
		}

		// if the parent gravity z is not zero OR we force to not have to zero gravity applied
		if(bAvoidZeroGravity || Super::GetGravityZ() != 0.f)
		{
			return FVector(0.f, 0.f, (Super::GetGravityZ() > 0.f ? 1.f : -1.f) * (GravityScale > 0.f ? 1.f : -1.f));
		}
	}

	//if we want the zero gravity to apply to the actor
	else if (bAvoidZeroGravity)
	{
		//want zero gravity, but have the directional gravity applied
		if(!CustomGravityDirection.IsZero()) return CustomGravityDirection;
		return FVector(0.f, 0.f, Super::GetGravityZ() > 0.f ? 1.f : -1.f);
	}

	return FVector::ZeroVector;
}

void USCharacterMovementComponent::SetGravityDirection(const FVector& NewGravityDirection)
{
	CustomGravityDirection = NewGravityDirection.GetSafeNormal();
}

FVector USCharacterMovementComponent::GetGravity() const
{
	if(!CustomGravityDirection.IsZero())
	{
		return CustomGravityDirection * FMath::Abs(Super::GetGravityZ() * GravityScale);
	}

	return FVector(0.f, 0.f, GetGravityZ());
}

FVector USCharacterMovementComponent::GetComponentDesiredAxisZ() const
{
	//Finding the correct up direction for the capsule
	return GetGravityDirection(true) * -1.f;
}

FQuat USCharacterMovementComponent::GetCapsuleRotation() const
{
	//Note that the UpdatedComponent is the owner of the MovementComp
	return UpdatedComponent->GetComponentQuat();
}

//https://cpp.hotexamples.com/examples/-/FQuat/RotateVector/cpp-fquat-rotatevector-method-examples.html
//https://forums.unrealengine.com/t/fquat-and-vector-rotation/67366
FVector USCharacterMovementComponent::GetCapsuleAxisX() const
{
	// Fast Simplification of RotateVector() with FVector(1,0,0)
	const FQuat CapsuleRotation = GetCapsuleRotation();
	const FVector QuatVector = FVector(CapsuleRotation.X, CapsuleRotation.Y, CapsuleRotation.Z);

	//https://stackoverflow.com/questions/42285413/why-do-quaternions-have-four-variables#:~:text=%22A%20quaternion%20is%20basically%20an,%2C%20y%2C%20z%20and%20w.
	return FVector(FMath::Square(CapsuleRotation.W) - QuatVector.SizeSquared(), CapsuleRotation.Z * CapsuleRotation.W * 2.f, CapsuleRotation.Y * CapsuleRotation.W * -2.f)
	+ QuatVector*(CapsuleRotation.X*2.f);
}

FVector USCharacterMovementComponent::GetCapsuleAxisZ() const
{
	// Fast Simplification of RotateVector() with FVector(0,0,1)
	const FQuat CapsuleRotation = GetCapsuleRotation();
	const FVector QuatVector = FVector(CapsuleRotation.X, CapsuleRotation.Y, CapsuleRotation.Z);

	return FVector(CapsuleRotation.Y * CapsuleRotation.W * 2.0f, CapsuleRotation.X * CapsuleRotation.W * -2.0f,
		FMath::Square(CapsuleRotation.W) - QuatVector.SizeSquared()) + QuatVector * (CapsuleRotation.Z * 2.0f);
}

FVector USCharacterMovementComponent::GetSafeNormalPrecise(const FVector& Vector) const
{
	//Magnitude
	const float VectorSquare = Vector.SizeSquared();
	if (VectorSquare < SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	return Vector*(1.f/FMath::Sqrt(VectorSquare));
}

void USCharacterMovementComponent::UpdateComponentRotation() const
{
	if(!UpdatedComponent)
	{
		return;
	}

	const FVector DesiredCapsuleUp = GetComponentDesiredAxisZ();

	//take desired z rotation axis of capsule, try to keep current x rotation of capsule
	const FMatrix RotationMatrix = FRotationMatrix::MakeFromZX(DesiredCapsuleUp, GetCapsuleAxisX());
	UpdatedComponent->MoveComponent(FVector::ZeroVector, RotationMatrix.Rotator(), true);
}

bool USCharacterMovementComponent::IsWithinEdgeToleranceNew(const FVector& CapsuleLocation, const FVector& CapsuleDown,
	const FVector& ImpactPoint, const float CapsuleRadius) const
{
	const float DistFromCenterSq = (CapsuleLocation + CapsuleDown * ((ImpactPoint - CapsuleLocation) | CapsuleDown) - ImpactPoint).SizeSquared();
	const float ReducedRadiusSq = FMath::Square(FMath::Max(KINDA_SMALL_NUMBER, CapsuleRadius - SWEEP_EDGE_REJECT_DISTANCE));

	return DistFromCenterSq < ReducedRadiusSq;
}
