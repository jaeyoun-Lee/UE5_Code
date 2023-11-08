// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"


// 생성자
UPlayerMove::UPlayerMove()
{
	// Tick함수가 호출되도록
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	// 초기 속도를 걷기로 설정
	moveComp->MaxWalkSpeed = walkSpeed;
}

void UPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Move();
}

void UPlayerMove::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMove::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMove::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMove::InputHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMove::InputVertical);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UPlayerMove::InputJump);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UPlayerMove::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released , this, &UPlayerMove::InputRun);

}

void UPlayerMove::Turn(float value)
{
	// Yaw방향(= Z축, 좌우)으로 회전 처리
	me->AddControllerYawInput(value);
}

void UPlayerMove::LookUp(float value)
{
	// Pitch 방향(= X축, 상하)
	me->AddControllerPitchInput(value);
}

void UPlayerMove::InputHorizontal(float value)
{
	dir.Y = value;
}

void UPlayerMove::InputVertical(float value)
{
	dir.X = value;
}

void UPlayerMove::InputJump()
{
	// Character 클래스의 기능 중 Jump()를 호출(클래스 내장 함수임)
	me->Jump();
	
}

void UPlayerMove::Move()
{
	// GetControlRoatation()을 이용하여 컨트롤러 회전 방향을 구함.
	// 회전 방향을 찾고, TransformVector()를 이용하여 특정 이동 벡터 dir를 자신의 기준(위에서 구한 회전 방향)으로 변환해준다
	dir = FTransform(me->GetControlRotation()).TransformVector(dir);
	// 플레이어 이동처리
	// 등속운동
	// ﻿P(결과 위치) = P0(현재 위치) + V(속도) * t(시간)
	//FVector P0 = GetActorLocation();
	//FVector vt = dir * walkSpeed * DeltaTime;	// v = 크기와 방향으로 이루어진 벡터로 사용자 입력으로 구한 dir * 걷는 속도
	//FVector P = P0 + vt;

	//SetActorLocation(P);	// Player 위치 변경

	me->AddMovementInput(dir);
	dir = FVector::ZeroVector;	// 방향 dir의 모든 요소(x,y,z)에 0을 할당하여 초기화
}

void UPlayerMove::InputRun()
{
	auto movement = moveComp;

	// 현재 달리기 모드라면 
	// 초기 세팅으로 MAXWalkSpeed = 200, walkSpeed = 200이다.
	// 즉, 걷고있을 때 shift를 누르면 else로 뛰는 모드가 된다.

	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}

	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}