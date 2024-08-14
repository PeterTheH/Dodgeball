// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBall.generated.h"

UCLASS()
class UE5TOPDOWNARPG_API ABaseBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBall();

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereComponent;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;

	UFUNCTION(NetMulticast, Reliable)
	void OnPickUp(USkeletalMeshComponent* skeletalMesh);
	UFUNCTION(NetMulticast, Reliable)
	void OnThrow(FVector Location);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle TimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool IsSupportedForNetworking() const override { return true; }


	

};
