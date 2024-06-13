#include "EnemyManager.h"
#include "Enemy.h"
#include <EngineUtils.h>
#include <Kismet/GameplayStatics.h>

AEnemyManager::AEnemyManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 1. ���� ���� �ð� ���ϱ�
	float createTime = FMath::RandRange(minTime, maxTime);
	// 2. Timer Manager���� �˶� ���
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this,
		&AEnemyManager::CreateEnemy, createTime);

	// ���� ��ġ ���� �Ҵ�
	FindSpawnPoints();
}

void AEnemyManager::CreateEnemy()
{
	// ���� ��ġ ���ϱ�
	int index = FMath::RandRange(0, spawnPoints.Num() - 1);
	// �� ���� �� ��ġ�ϱ�
	GetWorld()->SpawnActor<AEnemy>(enemyFactory, spawnPoints[index]->GetActorLocation(),
		FRotator(0));

	// �ٽ� ���� �ð��� CreateEnemy �Լ��� ȣ��ǵ��� Ÿ�̸� ����
	float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this,
		&AEnemyManager::CreateEnemy, createTime);
}

void AEnemyManager::FindSpawnPoints()
{
	// �˻����� ã�� ����� ������ �迭
	TArray<AActor*> allActors;
	// ���ϴ� Ÿ���� ���� ��� ã�ƿ���
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	// ã�� ����� ���� ��� �ݺ�������
	for (auto spawn : allActors)
	{
		// ã�� ������ �̸��� �ش� ���ڿ��� �����ϰ� �ִٸ�
		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
		{
			// ���� ��Ͽ� �߰�
			spawnPoints.Add(spawn);
		}
	}
}

//void AEnemyManager::FindSpawnPoints()
//{
//	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
//	{
//		AActor* spawn = *It;
//		// ã�� ������ �̸��� �ش� ���ڿ��� �����ϰ� �ִٸ�
//		if (spawn->GetName().Contains(TEXT("BP_EnemySpawnPoint")))
//		{
//			// ���� ��Ͽ� �߰�
//			spawnPoints.Add(spawn);
//		}
//	}
//}

