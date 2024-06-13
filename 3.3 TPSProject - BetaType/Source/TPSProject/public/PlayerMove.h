// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMove();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputAction* ia_Turn;

	// �¿� ȸ�� �Է� ó��
	void Turn(const struct FInputActionValue& inputValue);
	// ���� ȸ�� �Է� ó��
	void LookUp(const struct FInputActionValue& inputValue);

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	// �̵� ����
	FVector direction;

	void Move(const struct FInputActionValue& inputValue);

	// �̵� �ӵ�
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;
	// �޸��� �ӵ�
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;

	// �÷��̾� �̵� ó��
	void PlayerMove();

	// �޸��� �Է�
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;
	// �޸��� �̺�Ʈ ó���Լ�
	void InputRun();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;
	// ���� �Է� �̺�Ʈ ó�� �Լ�
	void InputJump(const struct FInputActionValue& inputValue);
};
