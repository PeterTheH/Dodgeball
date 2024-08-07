// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBall.h"
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
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("OverlapBegin %s %s"), *Other->GetName(), *OtherComp->GetName());
		MeshComponent->SetRenderCustomDepth(true);
	}
}

void ABaseBall::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (MeshComponent != nullptr)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("OverlapEnd %s %s"), *Other->GetName(), *OtherComp->GetName());
		MeshComponent->SetRenderCustomDepth(false);
	}
}

// Called every frame
void ABaseBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}