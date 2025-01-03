// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBall.h"
#include "../UE5TopDownARPGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../UE5TopDownARPG.h"
#include "../SavePlayerState.h"

// Called every frame
void ABaseBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
// Sets default values
ABaseBall::ABaseBall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bThrown = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphereComponent"));
	HitOverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("HitOverlapComponent"));

	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitOverlapComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitOverlapComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MeshComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = MeshComponent;

	SphereComponent->SetupAttachment(MeshComponent);
	HitOverlapComponent->SetupAttachment(MeshComponent);

	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseBall::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ABaseBall::OnEndOverlap);
	HitOverlapComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseBall::OnMeshHit);
	MeshComponent->OnComponentHit.AddDynamic(this, &ABaseBall::OnHit);
}

// Called when the game starts or when spawned
void ABaseBall::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseBall::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (MeshComponent != nullptr)
	{
		AUE5TopDownARPGCharacter* overlappedPlayer = Cast<AUE5TopDownARPGCharacter>(Other);

		if (overlappedPlayer)
		{
			MeshComponent->SetRenderCustomDepth(true);
			overlappedPlayer->queueBallsInRange.Enqueue(this);
			//UE_LOG(LogUE5TopDownARPG, Log, TEXT("OverlapBegin %s %s"), *Other->GetName(), *OtherComp->GetName());
		}
	}
}

void ABaseBall::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsHeld)
	{
		if (MeshComponent)
		{
			MeshComponent->SetRenderCustomDepth(false);

			AUE5TopDownARPGCharacter* overlappedPlayer = Cast<AUE5TopDownARPGCharacter>(Other);
			if (IsValid(overlappedPlayer))
			{

				if (overlappedPlayer->queueBallsInRange.Peek())
				{
					ABaseBall* front = *overlappedPlayer->queueBallsInRange.Peek();
					UE_LOG(LogUE5TopDownARPG, Log, TEXT("Ended Overlap with Ball: %s"), *front->GetName());
				}
				overlappedPlayer->queueBallsInRange.Pop();
			}
		}
	}
}

void ABaseBall::OnMeshHit(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AUE5TopDownARPGCharacter* overlappedPlayer = Cast<AUE5TopDownARPGCharacter>(Other);
	//AUE5TopDownARPGGameMode* GM = Cast<AUE5TopDownARPGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	//UE_LOG(LogUE5TopDownARPG, Log, TEXT("testington: %s"), *overlappedPlayer->GetName());
	if (IsValid(overlappedPlayer))
	{
		USkeletalMeshComponent* skeletalMesh = overlappedPlayer->FindComponentByClass<USkeletalMeshComponent>();

		if (IsValid(skeletalMesh) && bThrown && holdingPlayer && overlappedPlayer->bIsBlueTeam != holdingPlayer->bIsBlueTeam)
		{
			overlappedPlayer->bIsDead = true;
			skeletalMesh->SetCollisionProfileName("Ragdoll"); // TODO: remove collision box from character
			skeletalMesh->SetSimulatePhysics(true);
		}
	}
}

void ABaseBall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogUE5TopDownARPG, Log, TEXT("collision detected"));
	bThrown = false;
}

void ABaseBall::OnPickUp_Implementation(USkeletalMeshComponent* skeletalMesh, bool bIsDead)
{
	if (skeletalMesh->GetSocketByName("hand_lSocket") && !bIsDead)
	{
		MeshComponent->AttachToComponent(skeletalMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), "hand_lSocket");
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->SetRenderCustomDepth(false);
		MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		holdingPlayer = Cast<AUE5TopDownARPGCharacter>(skeletalMesh->GetOwner());
		holdingPlayer->bIsHoldingBall = true;

		bIsHeld = true;
	}
}

void ABaseBall::OnThrow_Implementation(FVector Location)
{
	FVector Direction = (Location - GetActorLocation()).GetSafeNormal();
	Direction.Z += 0.2;
	MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->AddImpulse(Direction * 2000, NAME_None, true);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, 
		[this]() { 
			MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); 
			bIsHeld = false;
			bThrown = true;}, 0.05f, false);

	if (holdingPlayer)
	{
		holdingPlayer->bIsHoldingBall = false;
		//holdingPlayer->queueBallsInRange.Pop();
		//UE_LOG(LogUE5TopDownARPG, Log, TEXT("Threw Ball"));
		//UE_LOG(LogTemp, Log, TEXT("IsQueue empty: %s"), holdingPlayer->queueBallsInRange.Peek() == nullptr ? TEXT("true") : TEXT("false"));

	}
}