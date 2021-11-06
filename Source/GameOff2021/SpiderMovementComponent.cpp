// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderMovementComponent.h"
#include "SpiderPawn.h"
#include "Components/CapsuleComponent.h"

USpiderMovementComponent::USpiderMovementComponent()
{
	MaxSpeed = 1000.f;
	Acceleration = 5000.f;
	Deceleration = 10000.f;
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

	UpdateVelocity(DeltaTime);
	UpdatePosition(DeltaTime);
}

float USpiderMovementComponent::GetMaxSpeed() const
{
	return MaxSpeed;
}

void USpiderMovementComponent::UpdateVelocity(float DeltaTime)
{
	const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);
	const float ControlInput = ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f;

	if (FMath::IsNearlyZero(ControlInput) && Velocity.SizeSquared() > 0.f)
	{
		// Slow down with no input
		Velocity = Velocity.GetSafeNormal() * FMath::Max(Velocity.Size() - FMath::Abs(Deceleration) * DeltaTime, 0.f);
	}

	Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);

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

		if (!bPositionCorrected)
		{
			const FVector NewLocation = UpdatedComponent->GetComponentLocation();
			Velocity = ((NewLocation - OldLocation) / DeltaTime);
		}
	}
}
