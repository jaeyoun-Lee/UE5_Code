// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include "Enemy.h"
#include "TPSProject.h"	// PRINT_LOG 사용하기 위해서
#include <Components/CapsuleComponent.h>	// 캡슐 컴포넌트 콜리전 비활성화를 위해
#include "EnemyAnim.h"
#include <AIController.h>
#include <NavigationSystem.h>


// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 ATPSPlayer 타겟 찾아오기
	// StaticClass() 함수로 이 함수를 실행한 클래스(ATPSPlayer)를 반환하고,
	// GetActorOfClass() 함수로 월드 내에서 전달 받은 클래스를 가진 액터를 하나 찾는다.
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());	
	
	// 발견한 액터를 ATPSPlayer로 캐스팅하여 target으로 지정
	target = Cast<ATPSPlayer>(actor);

	// 소유 객체 가져오기
	// GetOwner() 함수로 소유 액터 객체 가져와서 캐스팅하고 me로 지정
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* 할당 (ABP_Enemy 할당)
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());

	// AAIController 할당
	ai = Cast<AAIController>(me->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;

	case EEnemyState::Move:
		MoveState();
		break;

	case EEnemyState::Attack:
		AttackState();
		break;

	case EEnemyState::Damage:
		DamageState();
		break;

	case EEnemyState::Die:
		DieState();
		break;

	}
}

// 대기 상태
void UEnemyFSM::IdleState()
{
	// 일정 시간 기다렸다가 이동 상태로 전환하고 싶다.
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;	// 델타 타임을 계속 누적시킴

	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > idleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다.
		mState = EEnemyState::Move;

		// 경과 시간 초기화
		currentTime = 0;

		// 애니메이션 상태 동기화
		anim->animState = mState;

		// 최초 랜덤한 위치 정해주기
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// 이동 상태
void UEnemyFSM::MoveState()
{
	// 타겟(Player)쪽으로 이동하게 한다.
	// 그러다가 타겟이 가까워지면 공격 상태로 변경한다.
	
	// 1. 타겟의 목적지 가져오기
	FVector destination = target->GetActorLocation();

	// 2. 방향이 필요하다.
	FVector dir = destination - me->GetActorLocation();

	// 3. 그 방향으로 이동한다.
	//me->AddMovementInput(dir.GetSafeNormal());
	//ai->MoveToLocation(destination);

	// 내비게이션 시스템 인스턴스 얻기
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	// 목적지 길 찾기 경로 데이터 검색
	FPathFindingQuery query;
	FAIMoveRequest req;

	// 목적지에서 인지할 수 있는 범위 3cm
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);

	// 길 찾기를 위한 쿼리 생성
	ai->BuildPathfindingQuery(req, query);

	// 길 찾기 결과 가져오기
	FPathFindingResult r = ns->FindPathSync(query);

	// 목적지 길찾기 성공 여부 확인
	if (r.Result == ENavigationQueryResult::Success)
	{
		// 길찾기 성공하여 타깃 쪽으로 이동
		ai->MoveToLocation(destination);
	}
	
	else
	{
		// 길찾기 실패로 랜덤 위치 이동
		auto result = ai->MoveToLocation(randomPos);

		// 랜덤한 위치 목적지 도착 시,
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}


	// 타겟과 가까워 지면 공격 상태로 전환한다.
	// 1. 타겟과의 거리가 공격 범위 안에 들어온다면
	if (dir.Size() < attackRange)	// Size() 함수는 벡터 dir의 길이를 반환해줌
	{
		// 길 찾기 기능 정지
		ai->StopMovement();

		// 2. 공격 상태로 전환
		mState = EEnemyState::Attack;

		// 애니메이션 상태 동기화
		anim->animState = mState;

		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;

		// 공격 상태 전환 시, 대기 시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	}
}

// 공격 상태
void UEnemyFSM::AttackState()
{
	// 일정 시간마다 한 번 공격한다.
	// 1. 시간이 흐르다가
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 2. 공격 대기 시간을 초과하면
	if (currentTime > attackDelayTime)
	{
		//// 3. 공격한다.
		//PRINT_LOG(TEXT("Attack to Player"));

		// 경과 시간 초기화
		currentTime = 0;

		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;
	}

	// 타겟이 공격 범위를 벗어나면 상태를 이동으로 전환한다.
	// 1. 타겟과의 거리 구하기
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	// 2. 타겟과의 거리가 공격 범위를 넘어갔다면
	if (distance > attackRange)
	{
		mState = EEnemyState::Move;
		
		// 애니메이션 상태 동기화
		anim->animState = mState;

		// 최초 랜덤한 위치 정해주기
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// 피격 상태
void UEnemyFSM::DamageState() 
{
	// 1. 시간이 흐르다가
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 만약 경과 시간이 피격 대기시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		// 3. 대기 상태로 변환
		mState = EEnemyState::Idle;

		// 경과 시간 초기화
		currentTime = 0;

		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

// 죽음 상태
void UEnemyFSM::DieState()
{
	// 아직 죽음 애니메이션이 끝나지 않았다면
	// 바닥 내려가지 않도록 함.(바로 return해서 아래 선언부는 실행 X)
	if (anim->bDieDone == false)
	{
		return;
	}

	// 1. 계속 아래로 내려감
	// 등속 운동 공식 P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;

	me->SetActorLocation(P);

	// 2. 내려가다가 화면에서 안보이면(= 2m 내려가면) 제거한다.
	if (P.Z < -200.0f)
	{
		// 제거한다.
		me->Destroy();
	}
}


// 피격 알림 이벤트
// TPSPlayer.cpp의 InputFire() 에서 사용
void UEnemyFSM::OnDamageProcess()
{
	// 체력 1씩 감소
	hp--;	

	// 체력이 남아 있으면
	if (hp > 0)
	{
		// 피격 상태로 전환
		mState = EEnemyState::Damage;

		currentTime = 0;

		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);	// RandRange() 함수로 0과 1중 랜덤 값 하나 뽑음
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);	// 몽타주의 섹션이름 저장
		anim->PlayDamageAnim(FName(*sectionName));	// PlayDamgeAnim() 함수로 몽타주 섹션이름을 전달하여, 애니메이션 재생
													// 전달할 때, 매개변수의 자료형이 FName이므로 FName 값으로 변환해 저장
	}

	// 체력이 없다면
	else
	{
		// 죽음 상태로 전환
		mState = EEnemyState::Die;

		// 바닥으로 내려가기 전, 캡슐 충돌체 비활성화 해줘야한다.
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 죽음 애니메이션 재생
		anim->PlayDamageAnim(FName("Die"));
	}

	// 애니메이션 상태 동기화
	anim->animState = mState;
	// 움직임 정지
	ai->StopMovement();
}

// 랜덤 위치 가져오는 함수
bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());	// 내비게이션 시스템 인스턴스를 GetNavigationSystme()함수로 가져옴
	FNavLocation loc;	// 위치 정보 담을 변수
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);	// 가져온 내비게이션 시스템 인스턴스의 멤버 함수 GetRandomReachablePointInRadius() 호출
																					// 함수는 주어진 centerLocation 기준으로 radius영역 안에 랜덤으로 위치 정보를 가져와서
																					// loc 변수에 위치 정보를 담아줌

	dest = loc.Location;	// 이렇게 구한 랜덤 위치를 dest 변수에 할당

	return result;	// 정상적으로 정보를 가져왔는지 true/false 반환
}