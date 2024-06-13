#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. �浹ü ����ϱ�
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	// 2. �浹 �������� ����
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	// 3. �浹ü ũ�� ����
	collisionComp->SetSphereRadius(13);
	// 4. ��Ʈ�� ���
	RootComponent = collisionComp;
	// 5. �ܰ� ������Ʈ ����ϱ�
	// 6. �θ� ������Ʈ ����
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	bodyMeshComp->SetupAttachment(collisionComp);
	// 7. �浹 ��Ȱ��ȭ
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 8. �ܰ� ũ�� ����
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// �߻�ü ������Ʈ
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	// movement ������Ʈ�� ���Ž�ų ������Ʈ ����
	movementComp->SetUpdatedComponent(collisionComp);
	// �ʱ� �ӵ�
	movementComp->InitialSpeed = 5000;
	// �ִ� �ӵ�
	movementComp->MaxSpeed = 5000;
	// �ݵ� ����
	movementComp->bShouldBounce = true;
	// �ݵ� ��
	movementComp->Bounciness = 0.3f;

	// ���� �ð� �ֱ�
	InitialLifeSpan = 2.0f;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle deathTimer;
	//GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);

	GetWorld()->GetTimerManager().SetTimer(deathTimer,
		FTimerDelegate::CreateLambda([this]()->void
			{
				Destroy();
			}), 2.0f, false);
}

void ABullet::Die()
{
	Destroy();
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ������ Ư�� �Ӽ��� �����ϸ� ȣ��Ǵ� �̺�Ʈ �Լ�
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// speed ���� �����Ǿ����� üũ
	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		// ������Ÿ�� �����Ʈ ������Ʈ�� speed �� ����
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}

