// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SpiderPlayerState.generated.h"

UENUM(BlueprintType)
enum EDeathCause
{
	Starvation      UMETA(DisplayName = "Starvation"),
    Eaten			UMETA(DisplayName = "Eaten"),
 };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpiderDiedDelegate, const EDeathCause, DeathCause);


UCLASS()
class GAMEOFF2021_API ASpiderPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetHunger(const float& NewHunger);

	UFUNCTION(BlueprintPure)
	float GetHunger() const;
	
	UFUNCTION(BlueprintCallable)
	void Death(const EDeathCause& deathCause);

	UPROPERTY(BlueprintAssignable)
	FSpiderDiedDelegate SpiderDiedDelegate;

protected:
	float Hunger;
	
};
