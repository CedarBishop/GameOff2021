// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpiderMovementComponent.generated.h"

class UCapsuleComponent;

USTRUCT()
struct GAMEOFF2021_API FSpiderMovementFloor
{
	GENERATED_BODY()

public:

	void Set(const FHitResult& InHit)
	{
		FloorHitResult = InHit;
	}
	void Clear()
	{
		FloorHitResult.Reset();
	}

	const FHitResult& GetHitResult() const { return FloorHitResult; }

	bool IsValidFloor() const { return FloorHitResult.IsValidBlockingHit(); }

private:

	FHitResult FloorHitResult;
};

/**
 * 
 */
UCLASS()
class GAMEOFF2021_API USpiderMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:

	USpiderMovementComponent();

	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual float GetMaxSpeed() const override;

	UFUNCTION(BlueprintCallable)
	void StartJumping(bool bJumpImmediately = false);
	UFUNCTION(BlueprintCallable)
	void FinishJumping();
	UFUNCTION(BlueprintCallable)
	void CancelJump();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AirSpeedModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	float MinJumpPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	float MaxJumpPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	float JumpChargeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	bool bCanJumpInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump", AdvancedDisplay)
	float MaxSpeedWhileHoldingJumpScaler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	float AdditionalFloorTraceLength = 3.f;

protected:

	void UpdateVelocity(float DeltaTime);
	void UpdatePosition(float DeltaTime);

	bool CheckFloor(float DeltaTime, FHitResult& OutHitResult, float TraceLength);

	// If we can jump at this time
	bool CanJump() const;

	UPROPERTY()
	UCapsuleComponent* CapsuleComp;

	bool bPendingJump = false;
	bool bHoldingJump = false;
	float JumpHoldTimer = 0.f;

	// Ground to floor or wall
	bool bIsGrounded = true;

public:

	UFUNCTION(BlueprintPure)
	bool IsGrounded() const { return bIsGrounded;}

	UFUNCTION(BlueprintPure)
	bool IsHoldingJump() const { return bHoldingJump; }
};
