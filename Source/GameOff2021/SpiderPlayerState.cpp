// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPlayerState.h"

void ASpiderPlayerState::SetHunger(const float& NewHunger)
{
    Hunger = NewHunger;
    if (Hunger)
    {
        Death(EDeathCause::Starvation);
    }
}

float ASpiderPlayerState::GetHunger() const
{
    return Hunger;
}

void ASpiderPlayerState::Death(const EDeathCause& deathCause)
{
    SpiderDiedDelegate.Broadcast(deathCause);
}
