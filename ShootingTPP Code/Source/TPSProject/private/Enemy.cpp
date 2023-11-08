// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// EnemyFSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	// 1. 스켈레탈 메시 데이터로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));

	// 1-1. 데이터 로드 성공 시,
	if (tempMesh.Succeeded())
	{
		// 1-2. 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);

		// 1-3. 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));

		// 1-4. 메시 크기 설정
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}

	// 애니메이션 블루 프린트 할당하기
	// 경로를 붙여넣고 마지막에 _C 를 추가해줘야한다. 그래야 블루 프린트 클래스로 인식한다. 없으면 읽기 오류 발생 !!!
	// 
	// ﻿Assertion failed: InElementOwner [File:D:\build\++UE5\Sync\Engine\Source\Runtime\TypedElementFramework\Public\Elements\Framework\TypedElementOwnerStore.h] [Line: 131] Element owner passed to RegisterElementOwner was null for key (<GetTypedElementOwnerStoreKeyDebugString not implemented for key type>)! Element owners must be valid! 
	
	// 아래 주석처리한 코드는 위 에러 발생 원인코드이다. 
	//ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BP/ABP_Enemy.ABP_Enemy_C'"));

	//if (tempClass.Succeeded())
	//{
	//	GetMesh()->SetAnimInstanceClass(tempClass.Class);
	//}

	// 월드에 배치되거나 스폰될 때 자동으로
	// AIController부터 소유(Possese)될 수 있도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

