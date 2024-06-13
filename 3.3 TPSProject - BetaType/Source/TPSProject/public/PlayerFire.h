#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerFire.generated.h"

UCLASS(ClassGroup=(PlayerComponent), meta = (BlueprintSpawnableComponent))
class TPSPROJECT_API UPlayerFire : public UPlayerBaseComponent
{
	GENERATED_BODY()
	
public:
	UPlayerFire();

	virtual void BeginPlay() override;

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

	UPROPERTY( )
	class UCameraComponent* tpsCamComp;
	// �� ���̷�Ż�޽�
	UPROPERTY( )
	class USkeletalMeshComponent* gunMeshComp;
	// �������۰� ����ƽ�޽� �߰�
	UPROPERTY( )
	class UStaticMeshComponent* sniperGunComp;
public:
	// �Ѿ� ����
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;
	// �Ѿ� �߻� ó���Լ�
	void InputFire(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_GrenadeGun;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperGun;
	// ��ź�� ��������� ����
	bool bUsingGrenadeGun = true;
	// ��ź������ ����
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
	// �������۰����� ����
	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Sniper;
	// �������� ����ó���Լ�
	void SniperAim(const struct FInputActionValue& inputValue);
	// �������� ���� ������ ����
	bool bSniperAim = false;
	// �������� UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category=SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	// �������� UI ���� �ν��Ͻ�
	UPROPERTY( )
	class UUserWidget* _sniperUI;

	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditAnywhere, Category=BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	// �Ϲ� ���� ũ�ν����UI ����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	// ũ�ν���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	// ī�޶� ����ũ �������Ʈ�� ������ ����
	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	// �Ѿ� �߻� ����
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	class USoundBase* bulletSound;
};
