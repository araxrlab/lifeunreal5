#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerBaseComponent();

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

public:	
	// ����� �Է� ���� ó�� �Լ�
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) {};

	// ������Ʈ ���� ����
	UPROPERTY()
	ATPSPlayer* me;

	UPROPERTY()
	UCharacterMovementComponent* moveComp;
};
