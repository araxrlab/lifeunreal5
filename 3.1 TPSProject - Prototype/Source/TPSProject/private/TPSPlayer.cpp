#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include <Bullet.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "EnemyFSM.h"

ATPSPlayer::ATPSPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터를 불러오고 싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 2. Mesh 컴포넌트의 위치와 회전값을 설정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// 3. TPS 카메라를 붙이고 싶다.
	// 3-1. SpringArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	// 3-2. Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;

	// 2단 점프
	JumpMaxCount = 2;

	// 4. 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh());
	// 4-2. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 4-3. 데이터로드가 성공했다면
	if (TempGunMesh.Succeeded())
	{
		// 4-4. 스켈레탈메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// 4-5. 위치 조정하기
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	// 5-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh());
	// 5-2. 스태틱메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 5-3. 데이터로드가 성공했다면
	if (TempSniperMesh.Succeeded())
	{
		// 5-4. 스태틱메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 5-5. 위치 조정하기
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
		// 5-6. 크기 조정하기
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
}

void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}

	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	// 2. 일반 조준 UI 크로스헤어 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	// 3. 일반 조준 UI 등록
	_crosshairUI->AddToViewport();

	// 기본으로 스나이퍼건을 사용하도록 설정
	ChangeToSniperGun(FInputActionValue());
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}

void ATPSPlayer::PlayerMove()
{
	// 플레이어 이동 처리
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this,
			&ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this,
			&ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this,
			&ATPSPlayer::Move);
		// 점프 입력 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this,
			&ATPSPlayer::InputJump);
		// 총알 발사 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this,
			&ATPSPlayer::InputFire);
		// 총 교체 이벤트 처리함수 바인딩
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, 
			&ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, 
			&ATPSPlayer::ChangeToSniperGun);
		// 스나이퍼 조준 모드 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this,
			&ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this,
			&ATPSPlayer::SniperAim);
	}
}

// 스나이퍼 조준
void ATPSPlayer::SniperAim(const FInputActionValue& inputValue)
{
	// 스나이퍼건 모드가 아니라면 처리하지 않는다.
	if (bUsingGrenadeGun)
	{
		return;
	}
	// Pressed 입력 처리
	if (bSniperAim == false)
	{
		// 1. 스나이퍼 조준 모드 활성화
		bSniperAim = true;
		// 2. 스나이퍼조준 UI 등록
		_sniperUI->AddToViewport();
		// 3. 카메라의 시야각 Field Of View 설정
		tpsCamComp->SetFieldOfView(45.0f);
		// 4. 일반 조준 UI 제거
		_crosshairUI->RemoveFromParent();
	}
	// Released 입력 처리
	else
	{
		// 1. 스나이퍼 조준 모드 비활성화
		bSniperAim = false;
		// 2. 스나이퍼 조준 UI 화면에서 제거
		_sniperUI->RemoveFromParent();
		// 3. 카메라 시야각 원래대로 복원
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. 일반 조준 UI 등록
		_crosshairUI->AddToViewport();
	}
}

void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	if (bUsingGrenadeGun)
	{
		// 총알 발사 처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	// 스나이퍼건 사용 시
	else
	{
		// LineTrace 의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace 의 종료 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신(플레이어)는 충돌에서 제외
		params.AddIgnoredActor(this);
		// Channel 필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos,
			ECC_Visibility, params);
		// LineTrace가 부딪혔을 때
		if (bHit)
		{
			// 총알 파편 효과 트랜스폼
			FTransform bulletTrans;
			// 부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			// 1. 만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 2. 조준한 방향이 필요
				FVector dir = (endPos - startPos).GetSafeNormal();
				// 날려 버릴 힘(F = ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				// 3. 그 방향으로 날려 버리고 싶다.
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			// 부딪힌 대상이 적인지 판단하기
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::Move(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	// 상하 입력 이벤트 처리
	direction.X = value.X;
	// 좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

// 유탄총으로 변경
void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	// 유탄총 사용 중으로 체크
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

// 스나이퍼건으로 변경
void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}