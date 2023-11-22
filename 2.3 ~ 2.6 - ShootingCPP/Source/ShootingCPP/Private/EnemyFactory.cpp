// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFactory.h"
#include "EnemyActor.h"


AEnemyFactory::AEnemyFactory()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemyFactory::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 만일, 경과된 시간이 생성할 시간을 지났다면...
    if (currentTime > delayTime)
    {
        // 경과된 시간을 0초로 초기화한다.
        currentTime = 0;

        // enemy 변수에 할당된 블루프린트를 자신의 위치에 생성한다.
        AEnemyActor* spawnActor = GetWorld()->SpawnActor<AEnemyActor>(enemy,
            GetActorLocation(), GetActorRotation());

    }
    // 그렇지 않다면...
    else
    {
        // 현재 프레임의 경과 시간을 누적시킨다.
        //currentTime = currentTime + DeltaTime;
        currentTime += DeltaTime;
    }

}

