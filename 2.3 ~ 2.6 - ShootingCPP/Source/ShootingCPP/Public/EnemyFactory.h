// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyFactory.generated.h"

UCLASS()
class SHOOTINGCPP_API AEnemyFactory : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyFactory();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float delayTime = 2.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyActor> enemy;

private:
	float currentTime = 0;

};
