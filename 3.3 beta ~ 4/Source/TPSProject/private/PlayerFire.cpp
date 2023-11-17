// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "EnemyFSM.h"
#include <Camera/CameraComponent.h>
#include "PlayerAnim.h"

UPlayerFire::UPlayerFire()
{
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}

void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();

	tpsCamComp = me->tpsCamComp;
	gunMeshComp = me->gunMeshComp;
	sniperGunComp = me->sniperGunComp;

	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	// 2. 일반 조준 UI 크로스헤어 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	// 3. 일반 조준 UI 등록
	_crosshairUI->AddToViewport();

	// 기본으로 스나이퍼건을 사용하도록 설정
	ChangeToSniperGun(FInputActionValue());
}

void UPlayerFire::SetupInputBinding(class UEnhancedInputComponent* PlayerInput)
{
	// 총알발사 이벤트 처리함수 바인딩
	PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &UPlayerFire::InputFire);
	// 총 교체 이벤트 처리함수 바인딩
	PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &UPlayerFire::ChangeToSniperGun);
	// 스나이퍼 조준 모드 이벤트 처리 함수 바인딩
	PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &UPlayerFire::SniperAim);
	PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::InputFire(const FInputActionValue& inputValue)
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// 카메라셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// 유탄총 사용시
	if (bUsingGrenadeGun)
	{
		// 총알발사처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	// 스나이퍼건 사용시
	else
	{
		// LineTrace 의 시작위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		// LineTrace 의 종료위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// LineTrace 의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신(플레이어) 는 충돌에서 제외
		params.AddIgnoredActor(me);
		// Channel 필터를 이용한 LineTrace 충돌 검출 (충돌정보, 시작위치, 종료위치, 검출채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		// LineTrace 가 부딪혔을 때
		if (bHit)
		{
			// 총알파편효과 트렌스폼
			FTransform bulletTrans;
			// 부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알파편효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			// 1.	만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 2. 조준한 방향이 필요
				FVector dir = (endPos - startPos).GetSafeNormal();
				// 날려버릴 힘(F = ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				// 3. 그 방향으로 날려버리고 싶다.
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

// 유탄총으로 변경
void UPlayerFire::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	// 유탄총 사용중으로 체크
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);

	// 유탄총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

// 스나이퍼건으로 변경
void UPlayerFire::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);

	// 유탄총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

void UPlayerFire::SniperAim(const FInputActionValue& inputValue)
{
	// 스나이퍼건 모드가 아니라면 처리하지 않는다.
	if (bUsingGrenadeGun)
	{
		return;
	}
	// Pressed 입력처리
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
	// Released 입력처리
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
