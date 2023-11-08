// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 3��Ī ��� ������ ����
	enum class EControlMode
	{
		GTA,
		DIABLO,
		NPC
	};

	// ��� ���� �Լ�
	void SetControlMode(EControlMode ControlMode);

	EControlMode currentMode = EControlMode::GTA;	// ���� ��� ����
	FVector DirectionToMove = FVector::ZeroVector;	// ���� ���� ����

	float armLengthTo = 0.0f;							// springArm ���� ����
	FRotator armRotationTo = FRotator::ZeroRotator;	// springArm ȸ�� �� ����
	float armLengthSpeed = 0.0f;					// springArm ���� ���� �ӵ�
	float armRotationSpeed = 0.0f;					// springArm ȸ�� ���� �ӵ�

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	USpringArmComponent* springArm;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	UCameraComponent* camComp;

	// ���� ���̷�Ż �޽�
	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	USkeletalMeshComponent* weapon;

	// ���Ͱ� �����Ǵ� ������ �ľ�
	virtual void PostInitializeComponents() override;

	// ���� ���� ��, ���� �Ӽ� ���� �Լ�
	void AttackStartComboState();

	// ���� ���� ��, �Լ�
	void AttackEndComboState();

	// ���Ϳ� ������� �����ϴ� TakeDamage �Լ� �������̵�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// ���� ���� �Ǿ��� �� ����(�� �Լ��� READY ������Ʈ���� ����)
	//virtual void PossessedBy(AController* NewController) override;	

	// ���� �Է� �Լ�(AI Controller�� ����� �� �ֵ��� Private -> Public���� ����)
	void Attack();

	// ���� �ִϸ��̼� ������ �˸��� ��������Ʈ
	FOnAttackEndDelegate OnAttackEnd;

private:
	// �̵� �Է� ���� �Լ�
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);

	// ��� ���� �Լ�
	void ViewChange();

	// UAnimMontage���ڿ� bool ���ڸ� ���� ����Լ��� ��Ÿ�� ����� ������ Ÿ�̹� �ľ�
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	// �ִ� �ν��Ͻ��� ���� ȣ���ϹǷ� �ƿ� ������ ����
	UPROPERTY()
	class UABAnimInstance* ABAnim;


	// ĳ���Ͱ� ����� �� �ִ� �޺� ���� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;		// ���� �޺��� �̵� ���� ����

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;	// �޺� �Է� ����

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;			// �޺� ī������ �ִ�ġ

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;		// ���� ���� ���� �޺� ī����
	
	// ���� ������ ���� ��� ������ �Լ�
	void AttackCheck();

	// ���� ���� �� ���� ������ ��
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

public:
	bool CanSetWeapon();	// ���Ⱑ �ִ��� Ȯ���ϴ� �Լ�
	void SetWeapon(class AABWeapon* NewWeapon);	// ���Ⱑ ���ٸ� hand_rSocket�� ���⸦ �����ϴ� �Լ�

	// ���� ����ϴ� ���⸦ ��� �ִ� AABWeapon Ŭ���� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = weapon)
	class AABWeapon* CurrentWeapon;


	// ĳ���� ���ȿ� ���� �������� ���� ������Ʈ
	// UABCharacterStatComponent�� ���� ���� ���� ������Ʈ�̴�.
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* stat;

	// ü�¹� UI �����ϱ� ���� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* hpBarWidget;

	void SetCharacterState(ECharacterState NewState);	// ĳ������ ������Ʈ ����
	ECharacterState GetCharacterState() const;			// ĳ������ ������Ʈ ��������

	int32 GetExp() const;								// NPC�� �׾��� �� �ִ� ����ġ

	float GetFinalAttackRange() const;					// �÷��̾��� ���� ���� ��ȯ
	float GetFinalAttackDamage() const;					// �������� �� ���� ����� ��ȯ

private:
	void OnAssetLoadCompleted();		// �ּ��� �ε� �Ϸ�� ȣ��� �Լ�

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);	// �ּ� ���
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// �ּ� �񵿱�� �ε�

	int32 AssetIndex = 0;	// ĳ���� �ּ��� �ε���

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, META = (AllowPrivateAccess = true))
	ECharacterState CurrentState;		// ���� ĳ������ ������Ʈ�� ������ ����

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, META = (AllowPrivateAccess = true))
	bool bIsPlayer;						// �÷��̾����� ? NPC���� ? �����ϴ� ����

	UPROPERTY()
	class AABAIController* ABAIController;			// AI ��Ʈ�ѷ�

	UPROPERTY()
	class AABPlayerController* ABPlayerController;	// �÷��̾� ��Ʈ�ѷ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, META = (AllowPrivateAccess = true))
	float DeadTimer;	// DEAD ������Ʈ�� �� ��, ������ �Լ��� ȣ��� �ð�

	// DEAD ������Ʈ�� ���� ��, ����� Ÿ�̸� �ڵ�
	FTimerHandle DeadTimerHandle = {};

};
