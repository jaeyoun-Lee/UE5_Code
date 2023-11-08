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

	// 3인칭 모드 열거형 선언
	enum class EControlMode
	{
		GTA,
		DIABLO,
		NPC
	};

	// 모드 설정 함수
	void SetControlMode(EControlMode ControlMode);

	EControlMode currentMode = EControlMode::GTA;	// 현재 모드 설정
	FVector DirectionToMove = FVector::ZeroVector;	// 현재 벡터 설정

	float armLengthTo = 0.0f;							// springArm 길이 설정
	FRotator armRotationTo = FRotator::ZeroRotator;	// springArm 회전 값 설정
	float armLengthSpeed = 0.0f;					// springArm 길이 변경 속도
	float armRotationSpeed = 0.0f;					// springArm 회전 변경 속도

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	USpringArmComponent* springArm;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	UCameraComponent* camComp;

	// 무기 스켈레탈 메시
	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	USkeletalMeshComponent* weapon;

	// 액터가 생성되는 시점을 파악
	virtual void PostInitializeComponents() override;

	// 공격 시작 시, 관련 속성 지정 함수
	void AttackStartComboState();

	// 공격 종료 시, 함수
	void AttackEndComboState();

	// 액터에 대미지를 전달하는 TakeDamage 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 폰이 빙의 되었을 때 실행(이 함수는 READY 스테이트에서 구현)
	//virtual void PossessedBy(AController* NewController) override;	

	// 공격 입력 함수(AI Controller도 사용할 수 있도록 Private -> Public으로 변경)
	void Attack();

	// 공격 애니메이션 끝남을 알리는 델리게이트
	FOnAttackEndDelegate OnAttackEnd;

private:
	// 이동 입력 관련 함수
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);

	// 모드 변경 함수
	void ViewChange();

	// UAnimMontage인자와 bool 인자를 가진 멤버함수로 몽타주 재생이 끝나는 타이밍 파악
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	// 애님 인스턴스를 자주 호출하므로 아예 변수로 선언
	UPROPERTY()
	class UABAnimInstance* ABAnim;


	// 캐릭터가 사용할 수 있는 콤보 관련 변수
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;		// 다음 콤보로 이동 가능 여부

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;	// 콤보 입력 여부

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;			// 콤보 카운터의 최대치

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;		// 현재 실행 중인 콤보 카운터
	
	// 공격 범위에 있을 경우 실행할 함수
	void AttackCheck();

	// 공격 범위 및 공격 반지름 값
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

public:
	bool CanSetWeapon();	// 무기가 있는지 확인하는 함수
	void SetWeapon(class AABWeapon* NewWeapon);	// 무기가 없다면 hand_rSocket에 무기를 장착하는 함수

	// 현재 사용하는 무기를 담고 있는 AABWeapon 클래스 오브젝트
	UPROPERTY(VisibleAnywhere, Category = weapon)
	class AABWeapon* CurrentWeapon;


	// 캐릭터 스탯에 대한 관리를할 액터 컴포넌트
	// UABCharacterStatComponent는 직접 만든 액터 컴포넌트이다.
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* stat;

	// 체력바 UI 부착하기 위한 클래스
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* hpBarWidget;

	void SetCharacterState(ECharacterState NewState);	// 캐릭터의 스테이트 설정
	ECharacterState GetCharacterState() const;			// 캐릭터의 스테이트 가져오기

	int32 GetExp() const;								// NPC가 죽었을 때 주는 경험치

	float GetFinalAttackRange() const;					// 플레이어의 공격 범위 반환
	float GetFinalAttackDamage() const;					// 공격했을 때 최종 대미지 반환

private:
	void OnAssetLoadCompleted();		// 애셋이 로딩 완료시 호출될 함수

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);	// 애셋 경로
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;			// 애셋 비동기로 로딩

	int32 AssetIndex = 0;	// 캐릭터 애셋의 인덱스

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, META = (AllowPrivateAccess = true))
	ECharacterState CurrentState;		// 현재 캐릭터의 스테이트를 저장할 변수

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, META = (AllowPrivateAccess = true))
	bool bIsPlayer;						// 플레이어인지 ? NPC인지 ? 구분하는 변수

	UPROPERTY()
	class AABAIController* ABAIController;			// AI 컨트롤러

	UPROPERTY()
	class AABPlayerController* ABPlayerController;	// 플레이어 컨트롤러

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, META = (AllowPrivateAccess = true))
	float DeadTimer;	// DEAD 스테이트가 된 후, 수행할 함수가 호출될 시간

	// DEAD 스테이트가 됐을 때, 사용할 타이머 핸들
	FTimerHandle DeadTimerHandle = {};

};
