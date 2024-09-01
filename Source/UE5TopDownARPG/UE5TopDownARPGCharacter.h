// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GameFramework/Character.h"
#include "Ball/BaseBall.h"
#include "UE5TopDownARPGCharacter.generated.h"

UCLASS(Blueprintable)
class AUE5TopDownARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUE5TopDownARPGCharacter();

	virtual void PostInitializeComponents() override;

	// Called every frame.
	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	///** Returns TopDownCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	///** Returns CameraBoom subobject **/
	// class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE float GetHealth()const { return Health; }

	bool ActivateAbility(FVector Location);

	TQueue<ABaseBall*> queueBallsInRange;

	UFUNCTION(Server, Reliable)
	void PickUp();

	UFUNCTION(Server, Reliable)
	void Throw(FVector Location);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	bool bIsBlueTeam = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsHoldingBall = false;

	//UPROPERTY(ReplicatedUsing = OnRep_CollisionResponse)
	//TEnumAsByte<ECollisionResponse> CollisionResponse;

private:
	///** Top down camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* TopDownCameraComponent;

	///** Camera boom positioning the camera above the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* WidgetComponent;

	UPROPERTY()
	class UHealthbarWidget* HealthbarWidget;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class UBaseAbility* AbilityInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBaseAbility> AbilityTemplate;

	UPROPERTY(ReplicatedUsing = OnRep_SetHealth, EditDefaultsOnly)
	float Health = 100.0f;

	UPROPERTY(Replicated, EditDefaultsOnly)
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float DeathDelay = 1.0f;

	FTimerHandle DeathHandle;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> AfterDeathSpawnClass;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigateBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_SetHealth(float OldHealth);

	void Death();
};
