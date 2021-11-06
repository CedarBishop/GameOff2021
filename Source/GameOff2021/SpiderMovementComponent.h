// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpiderMovementComponent.generated.h"

class UCapsuleComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deceleration;

protected:

	void UpdateVelocity(float DeltaTime);
	void UpdatePosition(float DeltaTime);

	UPROPERTY()
	UCapsuleComponent* CapsuleComp;
};
