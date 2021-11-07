// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HungerComponent.generated.h"


UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class GAMEOFF2021_API UHungerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHungerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void EatFood(float HungerReplenished);

	UFUNCTION(BlueprintPure)
	float GetHunger() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float MaxHunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float TimeBetweenHungerIntervals;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunger")
	float HungerLossedPerInterval;

protected:

	UPROPERTY()
	float Hunger;

private:
	void LoseHunger();

	void HungerUpdated();

	FTimerHandle LoseHungerTimerHandle;
};
