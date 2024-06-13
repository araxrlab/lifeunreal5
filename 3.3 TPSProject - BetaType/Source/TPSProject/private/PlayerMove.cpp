// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

UPlayerMove::UPlayerMove()
{
	// Tick �Լ� ȣ��ǵ��� ó��
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ� �ӵ��� �ȱ�� ����
	moveComp->MaxWalkSpeed = walkSpeed;

}


void UPlayerMove::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PlayerMove();
}

void UPlayerMove::SetupInputBinding(class UEnhancedInputComponent* PlayerInput)
{
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this,
		&UPlayerMove::Turn);
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this,
		&UPlayerMove::LookUp); 
	PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this,
		&UPlayerMove::Move);
	// �޸��� �Է� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this,
		&UPlayerMove::InputRun);
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this,
		&UPlayerMove::InputRun);
	// ���� �Է� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this,
		&UPlayerMove::InputJump);
}

void UPlayerMove::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);
}

void UPlayerMove::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}

void UPlayerMove::Move(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	// ���� �Է� �̺�Ʈ ó��
	direction.X = value.X;
	// �¿� �Է� �̺�Ʈ ó��
	direction.Y = value.Y;
}

void UPlayerMove::PlayerMove()
{
	// �÷��̾� �̵� ó��
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void UPlayerMove::InputRun()
{
	auto movement = me->GetCharacterMovement();
	// ���� �޸��� �����
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// �ȱ� �ӵ��� ��ȯ
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

void UPlayerMove::InputJump(const struct FInputActionValue& inputValue)
{
	me->Jump();
}
