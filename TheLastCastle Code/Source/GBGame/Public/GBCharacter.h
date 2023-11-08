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
	virtual void PostInitializeComponents() override;	// 폰과 플레이어 컨트롤러가 생성되는 시점

	// 공격 당하여 받은 데미지 처리하는 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	float GetHPRatio();		// 현재 체력 비율 반환

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	UCameraComponent* camComp;			// 카메라 컴포넌트

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	USpringArmComponent* springArm;		// 스프링 암 컴포넌트

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	UCameraComponent* aimCamComp;			// 카메라 컴포넌트

	UPROPERTY(EditDefaultsOnly, Category = Cam)
	USpringArmComponent* aimSpringArm;		// 스프링 암 컴포넌트

	// 케이블 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class UCableComponent* cable;
	
	// 케이블 컴포넌트 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class UNiagaraComponent* shockLightning;

	// 와이어 사운드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class USoundBase* grapSound;

	// 와이어 사운드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable)
	class USoundBase* beAttackSFX;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool isJumping;			// 스페이스바를 눌러 점프한 것인지 ?

	// 전기 공격에 맞았는지 ?
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsShocking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool isDeath;			// 살아 있는지 ?

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Jump() override;		// Jump함수 오버라이드(공격 몽타주를 멈추고, 콤보 공격 초기화 코드 추가)

private:
	void CameraSetting();	// 카메라 세팅 함수(GTA 3인칭 시점)
	void animMontageSetting();	// 애님 몽타주 생성자에서 ConstructorHelpers 함수

	// 이동 관련 함수
	void UpDown(float value);		// 앞, 뒤 이동
	void LeftRight(float value);	// 좌, 우 이동
	void Turn(float value);			// 마우스로 좌, 우 시점이동
	void LookUp(float value);		// 마우스로 상, 하 시점이동

	void Run();				// 달리기 상태로 변환
	void Walk();			// 걷기 상태로 변환
	void Attack();			// 공격 
	void Shield();			// 방어
	void ShieldEnd();		// 방어 종료
	void AttackCheck();		// 공격 탐지

	void Grappling();			// 갈고리를 걸 수 있는 박스 콜리전이 있는지 찾음
	void GrapplingEnd();		// 갈고리 입력 종료

	UFUNCTION()
	void OnNextAttack();	// OnNextAttackCheck 델리게이트 호출되면 실행되는 함수
	
	UFUNCTION()
	void OnResetAttack();	// OnResetCheck 델리게이트 호출되면 실행되는 함수

	UFUNCTION()
	void OnEndShock();	// OnShockCheck 델리게이트 호출되면 실행되는 함수(쇼크 몽타주, 나이아가라 제거)
	
	UFUNCTION()
	void GameClear();	// 게임이 클리어되어 OnGameClaer 델리게이트가 호출되면 실행되는 함수

	UFUNCTION()
	void EndingCredit();	// 게임 클리어 후, 보여질 크레딧 함수

	UFUNCTION()
	void EndingCreditName();	// 게임 클리어 후, 이름이 보여질 크레딧 함수

	UFUNCTION()
	void EndingCreditUE();		// 게임 클리어 후, 언리얼 엔진 보여질 크레딧 함수

	UFUNCTION()
	void ReturnTitle();			// 게임 클리어 후, 게임 타이틀 화면으로 이동

	// priavte 멤버 변수 =====================================================
	UPROPERTY()
	class UGBAnimInstance* GBAnim;

	// Enemy가 player를 공격 가능한 상태인지 ?
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool canAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isGuarding;		// 가드 중인지 ?

	UPROPERTY(EditDefaultsOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float walkSpeed = 350.0f;		// 걷기 속도

	UPROPERTY(EditDefaultsOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float runSpeed = 700.0f;		// 뛰기 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool isComboInputOn;			// 콤보 공격을 할지 여부

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 currentCombo;				// 현재 공격 콤보

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRange;		// 공격 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float attackRadius;		// 공격 범위 반지름

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float max_HP;			// 최대 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float current_HP;		// 현재 체력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	FVector grabPoint;		// Grappling할 위치

	FVector GetUnitDirection(const FVector& Start, const FVector& End);		// 갈고리타고 날라갈 곳의 위치 정규화시켜주는 함수

	UPROPERTY()
	class AGBPlayerController* GBPlayerController;			// 플레이어 컨트롤러


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* currentMontage;		// 애니메이션 몽타주(공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_A;		// 애니메이션 몽타주(공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_B;		// 애니메이션 몽타주(공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_C;		// 애니메이션 몽타주(공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage_D;		// 애니메이션 몽타주(공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackAirMontage;		// 애니메이션 몽타주(공중 공격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ShieldMontage;		// 애니메이션 몽타주(막기)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* hitMontage;		// 애니메이션 몽타주(피격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* shockMontage;		// 애니메이션 몽타주(피격)를 저장할 변수

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* deathMontage;		// 애니메이션 몽타주(사망)를 저장할 변수

	// 크레딧이 올라갈 UI 올라오는 타이머
	FTimerHandle clearUIStartTimers;

	// 크레딧 올라오는 타이머
	FTimerHandle creditStartTimers;

	// 이름 크레딧 올라오는 타이머
	FTimerHandle creditNameTimers;

	// 언리얼 크레딧 올라오는 타이머
	FTimerHandle creditUETimers;

	// 타이틀 레벨로 돌아가는 타이머
	FTimerHandle returnTitle;

	// =========================================
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	int32 currentCombo;				// 현재 공격 콤보

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	int32 MaxCombo;					// 최대 공격 콤보

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	//	bool CanNextCombo;				// 다음 콤보로 넘어갈 수 있는지 여부

};
