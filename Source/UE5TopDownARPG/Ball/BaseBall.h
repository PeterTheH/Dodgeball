// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBall.generated.h"

class AUE5TopDownARPGCharacter;

UCLASS()
class UE5TOPDOWNARPG_API ABaseBall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseBall();

	virtual void Tick(float DeltaTime) override;
	virtual bool IsSupportedForNetworking() const override { return true; }

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereComponent;
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* HitOverlapComponent;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;
	UFUNCTION(NetMulticast, Reliable)
	void OnPickUp(USkeletalMeshComponent* skeletalMesh);
	UFUNCTION(NetMulticast, Reliable)
	void OnThrow(FVector Location);

	bool bThrown;

	UPROPERTY()
	AUE5TopDownARPGCharacter* holdingPlayer;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	FTimerHandle TimerHandle;
};
