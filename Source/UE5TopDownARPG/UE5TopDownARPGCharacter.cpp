// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TopDownARPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Abilities/BaseAbility.h"
#include "UE5TopDownARPGGameMode.h"
#include "UE5TopDownARPGPlayerController.h"
#include "UE5TopDownARPG.h"
#include "UI/HealthbarWidget.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "SavePlayerState.h"

AUE5TopDownARPGCharacter::AUE5TopDownARPGCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetCastShadow(false);
	WidgetComponent->SetReceivesDecals(false);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetupAttachment(RootComponent);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;
}

void AUE5TopDownARPGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(WidgetComponent->GetWidgetClass()))
	{
		WidgetComponent->InitWidget();
		HealthbarWidget = Cast<UHealthbarWidget>(WidgetComponent->GetUserWidgetObject());
	}
}

void AUE5TopDownARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		bIsBlueTeam = ASavePlayerState::GetPlayerTeam();

		ServerSetTeam(bIsBlueTeam);

		UE_LOG(LogUE5TopDownARPG, Warning, TEXT("IsLocallyControlled: bIsBlueTeam %d"), bIsBlueTeam);

	}

	if (AbilityTemplate != nullptr)
	{
		AbilityInstance = NewObject<UBaseAbility>(this, AbilityTemplate);
	}
	if (IsValid(HealthbarWidget))
	{
		float HealthPercent = Health / MaxHealth;
		HealthbarWidget->SetPercent(HealthPercent);
	}
}


void AUE5TopDownARPGCharacter::ServerSetTeam_Implementation(bool bNewIsBlueTeam)
{
	// This code runs on the server
	bIsBlueTeam = bNewIsBlueTeam;

	if (bIsBlueTeam)
	{
		SetActorLocation(FVector(0.0f, 700.0f, 100.0f));
	}
	else
	{
		SetActorLocation(FVector(0.0f, -700.0f, 100.0f));
	}

	// You can add server-side logic here if needed
	UE_LOG(LogTemp, Warning, TEXT("Server: bIsBlueTeam set to %d"), bIsBlueTeam);
}


void AUE5TopDownARPGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*
	FHitResult HitResult;
	FVector TraceStartLocation = GetActorLocation();
	FVector TraceEndLocation = GetActorLocation() + GetActorForwardVector() * 300.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECollisionChannel::ECC_WorldDynamic, Params))
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("TraceHit %s %s"), *HitResult.GetActor()->GetName(), *HitResult.GetComponent()->GetName());
	}
	*/
}

void AUE5TopDownARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUE5TopDownARPGCharacter, Health);
	DOREPLIFETIME(AUE5TopDownARPGCharacter, MaxHealth);
	DOREPLIFETIME(AUE5TopDownARPGCharacter, bIsBlueTeam);
}

bool AUE5TopDownARPGCharacter::ActivateAbility(FVector Location)
{
	if (IsValid(AbilityInstance))
	{
		return AbilityInstance->Activate(Location);
	}
	return false;
}

void AUE5TopDownARPGCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigateBy, AActor* DamageCauser)
{
	Health -= Damage;
	OnRep_SetHealth(Health + Damage);
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("Health %f"), Health);
	if (IsValid(HealthbarWidget))
	{
		float HealthPercent = Health / MaxHealth;
		HealthbarWidget->SetPercent(HealthPercent);
	}
	if (Health <= 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(DeathHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(DeathHandle, this, &AUE5TopDownARPGCharacter::Death, DeathDelay);
		}
	}
}

void AUE5TopDownARPGCharacter::OnRep_SetHealth(float OldHealth)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health %f"), Health));
	}
}

void AUE5TopDownARPGCharacter::Death()
{
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("Death"));
	AUE5TopDownARPGGameMode* GameMode = Cast<AUE5TopDownARPGGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GameMode))
	{
		GameMode->EndGame(false);
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	if (FMath::RandBool())
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor(AfterDeathSpawnClass, &Location, &Rotation, SpawnParameters);
	}

	GetWorld()->GetTimerManager().ClearTimer(DeathHandle);
	AUE5TopDownARPGPlayerController* PlayerController = Cast<AUE5TopDownARPGPlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		PlayerController->OnPlayerDied();
	}
	Destroy();
}

void AUE5TopDownARPGCharacter::PickUp_Implementation()
{
	USkeletalMeshComponent* skeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	if (skeletalMesh)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Picked Ball"));
		if (queueBallsInRange.Peek())
		{
			(*queueBallsInRange.Peek())->OnPickUp(skeletalMesh);
			queueBallsInRange.Dequeue(ABallInHand);
		}
	}
}

void AUE5TopDownARPGCharacter::Throw_Implementation(FVector Location)
{
	ABallInHand->OnThrow(Location);

	//USkeletalMeshComponent* skeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	//skeletalMesh->SetCollisionProfileName("Ragdoll");
	//skeletalMesh->SetSimulatePhysics(true);
}

void AUE5TopDownARPGCharacter::Restart()
{
	USkeletalMeshComponent* skeletalMesh = FindComponentByClass<USkeletalMeshComponent>();
	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	if (IsValid(skeletalMesh))
	{
		skeletalMesh->SetCollisionProfileName("CharacterMesh");
		skeletalMesh->SetSimulatePhysics(false);

	}
}