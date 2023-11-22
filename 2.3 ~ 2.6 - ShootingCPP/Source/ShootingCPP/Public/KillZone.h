// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

UCLASS()
class SHOOTINGCPP_API AKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AKillZone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;

};
