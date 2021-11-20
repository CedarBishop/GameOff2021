// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawn.h"
#include "SpiderMovementComponent.h"
#include "HungerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"

ASpiderPawn::ASpiderPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(50.f, 100.f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	SetRootComponent(CapsuleComponent);

	MovementComponent = CreateDefaultSubobject<USpiderMovementComponent>(TEXT("MovementComponent"));
	
	HungerComponent = CreateDefaultSubobject<UHungerComponent>(TEXT("HungerComponent"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ASpiderPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpiderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpiderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpiderPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpiderPawn::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ASpiderPawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpiderPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpiderPawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpiderPawn::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ASpiderPawn::JumpHold);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ASpiderPawn::JumpRelease);
}

#include "DrawDebugHelpers.h"

void ASpiderPawn::MoveForward(float Value)
{
	if ((Controller != nullptr))// && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector Direction = FMatrix(FollowCamera->GetForwardVector(), FollowCamera->GetRightVector(), MovementComponent->GetFloorNormal(), FVector4(0, 0, 0, 1)).GetUnitAxis(EAxis::X);
		

		//Direction = YawRotation.Vector();
		Direction = Direction.VectorPlaneProject(Direction, MovementComponent->GetFloorNormal());
		//DrawDebugCoordinateSystem(GetWorld(), GetActorLocation(), Direction.Rotation(), 150.f, false, -1.f, 20, 5.f);
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, -1.f, FColor::Green, Direction.ToString());
		}
		if (Value != 0.f)
			AddMovementInput(Direction, Value);
	}
}

void ASpiderPawn::MoveRight(float Value)
{
	if ((Controller != nullptr))// && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		FVector Direction = FMatrix(FollowCamera->GetForwardVector(), FollowCamera->GetRightVector(), MovementComponent->GetFloorNormal(), FVector4(0, 0, 0, 1)).GetUnitAxis(EAxis::X);
		// Hack
		float Angle = FVector::DotProduct(FVector::UpVector, MovementComponent->GetFloorNormal()) < -0.95f ? -90.f : 90.f;
		Direction = Direction.RotateAngleAxis(Angle, MovementComponent->GetFloorNormal());
		Direction = Direction.VectorPlaneProject(Direction, MovementComponent->GetFloorNormal());
		// add movement in that direction
		//DrawDebugCoordinateSystem(GetWorld(), GetActorLocation(), Direction.Rotation(), 300.f, false, -1.f, 20, 10.f);
		if (GEngine)

		{
			//GEngine->AddOnScreenDebugMessage(INDEX_NONE, -1.f, FColor::Green, Direction.ToString());
		}
		
		if (Value != 0.f)
			AddMovementInput(Direction, Value);
	}
}

void ASpiderPawn::TurnAtRate(float Rate)
{

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpiderPawn::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASpiderPawn::JumpHold()
{
	MovementComponent->StartJumping();
}

void ASpiderPawn::JumpRelease()
{
	MovementComponent->FinishJumping();
}

bool ASpiderPawn::IsGrounded() const
{
	return MovementComponent->IsGrounded();
}

