// Fill out your copyright notice in the Description page of Project Settings.


#include "HungerComponent.h"


#include "SpiderPlayerState.h"
#include "Kismet/GameplayStatics.h"

UHungerComponent::UHungerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHungerComponent::BeginPlay()
{
	Super::BeginPlay();

	Hunger = MaxHunger;
	HungerUpdated();

	UWorld* world = GetWorld();
	if (world)
	{
		world->GetTimerManager().SetTimer(LoseHungerTimerHandle, this, &UHungerComponent::LoseHunger,
			TimeBetweenHungerIntervals, true, TimeBetweenHungerIntervals);
	}
}

void UHungerComponent::EatFood(float HungerReplenished)
{
	Hunger += HungerReplenished;
	if (Hunger > MaxHunger)
	{
		Hunger = MaxHunger;
	}
	HungerUpdated();
}

float UHungerComponent::GetHunger() const
{
	return Hunger;
}

void UHungerComponent::LoseHunger()
{
	Hunger -= HungerLossedPerInterval;
	HungerUpdated();
}

void UHungerComponent::HungerUpdated()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!playerController)
	{
		return;
	}
	APlayerState* playerState = playerController->PlayerState.Get();
	if (!playerState)
	{
		return;
	}
	ASpiderPlayerState* spiderPlayerState = Cast<ASpiderPlayerState>(playerState);
	if (!spiderPlayerState)
	{
		return;
	}
	spiderPlayerState->SetHunger(Hunger);
}


