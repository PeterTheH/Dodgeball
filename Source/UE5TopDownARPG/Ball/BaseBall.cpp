// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBall.h"
#include "../UE5TopDownARPGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../UE5TopDownARPG.h"

// Sets default values
ABaseBall::ABaseBall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphereComponent"));
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = MeshComponent;

	SphereComponent->SetupAttachment(MeshComponent);

	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseBall::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ABaseBall::OnEndOverlap);
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
		MeshComponent->SetRenderCustomDepth(true);

		AUE5TopDownARPGCharacter* overlappedPlayer = Cast<AUE5TopDownARPGCharacter>(Other);
		if (overlappedPlayer)
		{
			overlappedPlayer->ptrBallInRange = this;
			//USkeletalMeshComponent* skeletalMesh = overlappedPlayer->FindComponentByClass<USkeletalMeshComponent>();
			//if (skeletalMesh)
			//{
			//	if (skeletalMesh->GetSocketByName("hand_lSocket"))
			//	{
			//		MeshComponent->SetRenderCustomDepth(false);
			//		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//		MeshComponent->AttachToComponent(skeletalMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
			//			EAttachmentRule::KeepWorld,
			//			EAttachmentRule::KeepWorld, true), "hand_lSocket");
			//		UE_LOG(LogUE5TopDownARPG, Log, TEXT("socketTest %s %s"), *Other->GetName(), *OtherComp->GetName());
			//	}

			//}



			//MeshComponent->AttachToComponent(Cast<USceneComponent>(Other), FAttachmentTransformRules::SnapToTargetNotIncludingScale);;
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("OverlapBegin %s %s"), *Other->GetName(), *OtherComp->GetName());
		}
	}
}

void ABaseBall::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (MeshComponent != nullptr)
	{
		MeshComponent->SetRenderCustomDepth(false);

		AUE5TopDownARPGCharacter* overlappedPlayer = Cast<AUE5TopDownARPGCharacter>(Other);
		if (overlappedPlayer)
		{
			overlappedPlayer->ptrBallInRange = nullptr;
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("OverlapEnd %s %s"), *Other->GetName(), *OtherComp->GetName());

		}
	}
}

// Called every frame
void ABaseBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}