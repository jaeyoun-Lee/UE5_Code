// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/Character.h"
#include "GBCharacter.generated.h"

UCLASS()
class GBGAME_API AGBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGBCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;	// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����

	// ���� ���Ͽ� ���� ������ ó���ϴ� �Լ�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	float GetHPRatio();		// ���� ü�� ���� ��ȯ

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	UCameraComponent* camComp;			// ī�޶� ������Ʈ

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	USpringArmComponent* springArm;		// ������ �� ������Ʈ

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	UCameraComponent* aimCamComp;			// ī�޶� ������Ʈ

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	USpringArmComponent* aimSpringArm;		// ������ �� ������Ʈ

	// ���̺� ������Ʈ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class UCableComponent* cable;
	
	// ���̺� ������Ʈ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class UNiagaraComponent* shockLightning;

	// ���̾� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class USoundBase* grapSound;

	// ���̾� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class USoundBase* beAttackSFX;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool isJumping;			// �����̽��ٸ� ���� ������ ������ ?

	// ���� ���ݿ� �¾Ҵ��� ?
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsShocking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool isDeath;			// ��� �ִ��� ?

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Jump() override;		// Jump�Լ� �������̵�(���� ��Ÿ�ָ� ���߰�, �޺� ���� �ʱ�ȭ �ڵ� �߰�)

private:
	void CameraSetting();	// ī�޶� ���� �Լ�(GTA 3��Ī ����)
	void animMontageSetting();	// �ִ� ��Ÿ�� �����ڿ��� ConstructorHelpers �Լ�

	// �̵� ���� �Լ�
	void UpDown(float value);		// ��, �� �̵�
	void LeftRight(float value);	// ��, �� �̵�
	void Turn(float value);			// ���콺�� ��, �� �����̵�
	void LookUp(float value);		// ���콺�� ��, �� �����̵�

	void Run();				// �޸��� ���·� ��ȯ
	void Walk();			// �ȱ� ���·� ��ȯ
	void Attack();			// ���� 
	void Shield();			// ���
	void ShieldEnd();		// ��� ����
	void AttackCheck();		// ���� Ž��

	void Grappling();			// ������ �� �� �ִ� �ڽ� �ݸ����� �ִ��� ã��
	void GrapplingEnd();		// ���� �Է� ����

	UFUNCTION()
	void OnNextAttack();	// OnNextAttackCheck ��������Ʈ ȣ��Ǹ� ����Ǵ� �Լ�
	
	UFUNCTION()
	void OnResetAttack();	// OnResetCheck ��������Ʈ ȣ��Ǹ� ����Ǵ� �Լ�

	UFUNCTION()
	void OnEndShock();	// OnShockCheck ��������Ʈ ȣ��Ǹ� ����Ǵ� �Լ�(��ũ ��Ÿ��, ���̾ư��� ����)
	
	UFUNCTION()
	void GameClear();	// ������ Ŭ����Ǿ� OnGameClaer ��������Ʈ�� ȣ��Ǹ� ����Ǵ� �Լ�

	UFUNCTION()
	void EndingCredit();	// ���� Ŭ���� ��, ������ ũ���� �Լ�

	UFUNCTION()
	void EndingCreditName();	// ���� Ŭ���� ��, �̸��� ������ ũ���� �Լ�

	UFUNCTION()
	void EndingCreditUE();		// ���� Ŭ���� ��, �𸮾� ���� ������ ũ���� �Լ�

	UFUNCTION()
	void ReturnTitle();			// ���� Ŭ���� ��, ���� Ÿ��Ʋ ȭ������ �̵�

	// priavte ��� ���� =====================================================
	UPROPERTY()
	class UGBAnimInstance* GBAnim;

	// Enemy�� player�� ���� ������ �������� ?
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool canAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isGuarding;		// ���� ������ ?

	UPROPERTY(EditDefaultsOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float walkSpeed = 350.0f;		// �ȱ� �ӵ�

	UPROPERTY(EditDefaultsOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float runSpeed = 700.0f;		// �ٱ� �ӵ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isComboInputOn;			// �޺� ������ ���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 currentCombo;				// ���� ���� �޺�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRange;		// ���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRadius;		// ���� ���� ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float max_HP;			// �ִ� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float current_HP;		// ���� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	FVector grabPoint;		// Grappling�� ��ġ

	FVector GetUnitDirection(const FVector& Start, const FVector& End);		// ����Ÿ�� ���� ���� ��ġ ����ȭ�����ִ� �Լ�

	UPROPERTY()
	class AGBPlayerController* GBPlayerController;			// �÷��̾� ��Ʈ�ѷ�


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* currentMontage;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_A;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_B;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_C;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_D;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackAirMontage;		// �ִϸ��̼� ��Ÿ��(���� ����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ShieldMontage;		// �ִϸ��̼� ��Ÿ��(����)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* hitMontage;		// �ִϸ��̼� ��Ÿ��(�ǰ�)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* shockMontage;		// �ִϸ��̼� ��Ÿ��(�ǰ�)�� ������ ����

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* deathMontage;		// �ִϸ��̼� ��Ÿ��(���)�� ������ ����

	// ũ������ �ö� UI �ö���� Ÿ�̸�
	FTimerHandle clearUIStartTimers;

	// ũ���� �ö���� Ÿ�̸�
	FTimerHandle creditStartTimers;

	// �̸� ũ���� �ö���� Ÿ�̸�
	FTimerHandle creditNameTimers;

	// �𸮾� ũ���� �ö���� Ÿ�̸�
	FTimerHandle creditUETimers;

	// Ÿ��Ʋ ������ ���ư��� Ÿ�̸�
	FTimerHandle returnTitle;

	// =========================================
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	int32 currentCombo;				// ���� ���� �޺�

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	int32 MaxCombo;					// �ִ� ���� �޺�

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	bool CanNextCombo;				// ���� �޺��� �Ѿ �� �ִ��� ����

};
