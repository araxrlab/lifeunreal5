// Fill out your copyright notice in the Description page of Project Settings.


#include "KillZone.h"
#include "Components/BoxComponent.h"


AKillZone::AKillZone()
{
	PrimaryActorTick.bCanEverTick = true;

    // 박스 컴포넌트를 생성하고 루트 컴포넌트로 설정한다.
    boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
    SetRootComponent(boxComp);

    // 박스 컴포넌트의 모빌리티를 고정 상태로 설정한다.
    boxComp->SetMobility(EComponentMobility::Static);

    // 박스의 크기를 50, 2000, 50으로 설정한다.
    boxComp->SetBoxExtent(FVector(50, 2000, 50));

    // 박스 컴포넌트의 콜리전 프리셋을 KillZone으로 설정한다.
    boxComp->SetCollisionProfileName(TEXT("KillZone"));

}

void AKillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKillZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

