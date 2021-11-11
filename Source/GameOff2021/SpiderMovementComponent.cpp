// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderMovementComponent.h"
#include "SpiderPawn.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

USpiderMovementComponent::USpiderMovementComponent()
{
	MaxSpeed = 1000.f;
	Acceleration = 5000.f;
	Deceleration = 10000.f;
	AirSpeedModifier = 0.8f;
	bCanJumpInAir = false;
	MinJumpPower = 200.f;
	MaxJumpPower = 800.f;
	JumpChargeTime = 1.f;
	MaxSpeedWhileHoldingJumpScaler = 0.2f;

	CapsuleComp = nullptr;

	bPendingJump = false;
}

void USpiderMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	CapsuleComp = Cast<UCapsuleComponent>(NewUpdatedComponent);
}

void USpiderMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	if (bHoldingJump)
	{
		if (CanJump())
		{
			JumpHoldTimer += DeltaTime;
		}
		else
		{
			CancelJump();
		}
	}

	UpdateVelocity(DeltaTime);
	UpdatePosition(DeltaTime);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, DeltaTime, FColor::Green, FString::Printf(TEXT("Is Grounded: %i"), (int32)bIsGrounded));
	}
}

float USpiderMovementComponent::GetMaxSpeed() const
{
	float Value = MaxSpeed;
	if (bHoldingJump)
	{
		Value *= MaxSpeedWhileHoldingJumpScaler;
	}

	return Value;
}

void USpiderMovementComponent::StartJumping(bool bJumpImmediately)
{
	if (bPendingJump)
	{
		return;
	}

	if (!CanJump())
	{
		return;
	}

	bHoldingJump = true;
	JumpHoldTimer = 0.f;

	if (bJumpImmediately)
	{
		FinishJumping();
	}
}

void USpiderMovementComponent::FinishJumping()
{
	if (bHoldingJump)
	{
		bHoldingJump = false;
		bPendingJump = true;
	}
}

void USpiderMovementComponent::CancelJump()
{
	bHoldingJump = false;
	bPendingJump = false;
	JumpHoldTimer = 0.f;
}

void USpiderMovementComponent::UpdateVelocity(float DeltaTime)
{
	if (bIsGrounded)
	{
		const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);
		const float ControlInput = ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f;

		// TODO: Only apply if grounded (or on wall)
		if (FMath::IsNearlyZero(ControlInput) && Velocity.SizeSquared() > 0.f)
		{
			// Slow down with no input
			Velocity = Velocity.GetSafeNormal() * FMath::Max(Velocity.Size() - FMath::Abs(Deceleration) * DeltaTime, 0.f);
		}

		Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(GetMaxSpeed());

		// TODO: Need to stick to surface (e.g, we might be going down a slope which we should walk down)
	}
	else
	{
		const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);
		const float ControlInput = ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f;

		Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime * AirSpeedModifier;
		float OldZ = Velocity.Z;
		Velocity.Z = 0.f;
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
		Velocity.Z = OldZ;

		Velocity.Z += GetGravityZ() * DeltaTime;
	}

	if (bPendingJump && CanJump())
	{
		float JumpPower = MaxJumpPower;
		if (JumpChargeTime > 0.f)
		{
			JumpPower = FMath::Lerp(MinJumpPower, MaxJumpPower, FMath::Clamp(JumpHoldTimer / JumpChargeTime, 0.f, 1.f));
		}

		bIsGrounded = false;
		Velocity.Z = JumpPower;
		bPendingJump = false;
		
	}

	ConsumeInputVector();
}

void USpiderMovementComponent::UpdatePosition(float DeltaTime)
{
	bool bPositionCorrected = false;

	// Move actor
	FVector Delta = Velocity * DeltaTime;

	if (!Delta.IsNearlyZero(1e-6f))
	{
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat Rotation = UpdatedComponent->GetComponentQuat();

		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);

		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, DeltaTime, Delta);
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}
	}

	bool bWasPreviouslyGrounded = bIsGrounded;

	FHitResult FloorHit;
	if (CheckFloor(DeltaTime, FloorHit, 0.f))
	{
		if (!bWasPreviouslyGrounded)
		{
			Velocity.Z = 0.f;
		}

		bIsGrounded = true;
	}
	else
	{
		bIsGrounded = false;
	}
}

bool USpiderMovementComponent::CheckFloor(float DeltaTime, FHitResult& OutHitResult, float TraceLength)
{
	if (!CapsuleComp)
	{
		return false;
	}

	float CapsuleRadius, CapsuleHalfHeight;
	CapsuleComp->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	// We use up vector so we stick to walls
	FVector TraceDirection = -CapsuleComp->GetUpVector();

	const float FinalTraceLength = TraceLength + AdditionalFloorTraceLength;
	FVector TraceStart = CapsuleComp->GetComponentLocation();
	FVector TraceEnd = TraceStart + (TraceDirection * FinalTraceLength);
	FQuat TraceRotation = CapsuleComp->GetComponentQuat();

	FCollisionQueryParams QueryParams(TEXT("SpiderMovementFloorCheck"), false, GetOwner());
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	UWorld* World = GetWorld();
	bool bBlockingHit = World->SweepSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceRotation, CollisionChannel, CollisionShape, QueryParams, ResponseParam);
	if (bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), OutHitResult.ImpactPoint, 50.f, 8, FColor::Red, false, -1.f, 10, 5.f);

		if (bIsGrounded)
		{
			return true;
		}
		else
		{
			// We are falling and may have just hit the floor, check though that its actually a floor and not a wall
			FVector ImpactPointRelative = CapsuleComp->GetComponentTransform().InverseTransformPosition(OutHitResult.ImpactPoint);
			return ImpactPointRelative.SizeSquared2D() < FMath::Square(0.2f);
		}
	}

	return false;
}

bool USpiderMovementComponent::CanJump() const
{
	return bIsGrounded || bCanJumpInAir;
}
