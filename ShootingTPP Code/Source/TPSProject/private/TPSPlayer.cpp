// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>	// SpringArm 컴포넌트를 사용하기 위해서 선언
#include <Camera/CameraComponent.h>	// Camera 컴포넌트를 사용하기 위해서 선언
#include <Kismet/GameplayStatics.h>
#include "PlayerMove.h"
#include "PlayerFire.h"
#include "TPSProject.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//	1. 스켈레탈 메시 데이터를 불러온다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Mixamo/Mixamo_Skeletal.Mixamo_Skeletal'"));

	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		//	2. Mesh 컴포넌트의 위치와 회전 값을 설정한다.	
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -80), FRotator(0, -90, 0));
	}
	
	// 3. TPSPlayer에 카메라 붙이기
	// 3-1. SpringArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Comp"));	// SpringArm컴포넌트 인스턴스 생성
	springArmComp->SetupAttachment(RootComponent);			// SpringArm 컴포넌트를 RootComponent의 자식 컴포넌트로 등록
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));	// SpringArm의 상대위치 설정
	springArmComp->TargetArmLength = 400;					// 타겟과의 지정거리
	springArmComp->bUsePawnControlRotation = true;			// 폰 제어 회전 사용 True

	// 3-2. Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TPS Cam Comp"));	// Camera컴포넌트 인스턴스 생성
	tpsCamComp->SetupAttachment(springArmComp);										// 카메라 컴포넌트를 SpringArmComp의 자식 컴포넌트로 등록
	tpsCamComp->bUsePawnControlRotation = false;									// 폰 제어 회전 사용 False

	bUseControllerRotationYaw = true;	// TPSPlayer의 컨트롤러 회전 요 사용 True

	// 2단 점프
	JumpMaxCount = 2;

	// 4. 총 스켈레탈 메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));

	//// 4-1. 부모 컴포넌트를 Mesh컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh(), FName("hand_rSocket"));

	// 4-2. 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	// 4-3. 스켈레탈 메시 데이터 로드 성공 시,
	if (TempGunMesh.Succeeded())
	{
		// 4-4. 스켈레탈 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);

		// 4-5. 위치 조정
		gunMeshComp->SetRelativeLocation(FVector(0, 2, -10));	// 4-5. 유탄총 위치 조정
		gunMeshComp->SetRelativeRotation(FRotator(70, 270, 80));	// 4-5. 유탄총 회전 조정
	}

	// 5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper"));
	
	// 5-1. 부모 컴포넌트를 메시 컴포넌트로 지정
	sniperGunComp->SetupAttachment(GetMesh(), FName("hand_rSocket"));

	// 5-2. 스태틱 메시 데이터로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));

	if (TempSniper.Succeeded())	// 5-3. 데이터 로드 성공 시,
	{
		sniperGunComp->SetStaticMesh(TempSniper.Object);	// 5-4. 스나이퍼 총 스태틱 메시 할당

		sniperGunComp->SetRelativeLocation(FVector(21, -32, -25)); // 5-5. 스나이퍼 총 위치 조정
		sniperGunComp->SetRelativeRotation(FRotator(65, -84, 79)); // 5-5. 스나이퍼 총 회전 조정

		sniperGunComp->SetRelativeScale3D(FVector(0.15f));	// 5-6. 스나이퍼 총 크기 조정
	}

	// 플레이어 이동 컴포넌트 등록
	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));

	//// 플레이어 공격 컴포넌트 등록
	//playerFire = CreateDefaultSubobject<UPlayerFire>(TEXT("PlayerFire"));

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	hp = initialHP;
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Move();
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 컴포넌트에서 입력 바인딩 처리하도록 호출
	onInputBindingDelegate.Broadcast(PlayerInputComponent);

	//// 컴포넌트에서 입력 바인딩 처리하도록 호출
	//playerMove->SetupInputBinding(PlayerInputComponent);
	//playerFire->SetupInputBinding(PlayerInputComponent);
}

void ATPSPlayer::OnHitEvent()
{
	PRINT_LOG(TEXT("Damaged !"));
	hp--;

	if (hp <= 0)
	{
		PRINT_LOG(TEXT("Player is Dead..."));
		OnGameOver();
	}
}

void ATPSPlayer::OnGameOver_Implementation()
{
	// 게임 오버 시, 일시 정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}


// 총 교체 시, 호출되는 이벤트
void ATPSPlayer::OnUsingGrenade_Implementation(bool isGrenade)
{
	
}
