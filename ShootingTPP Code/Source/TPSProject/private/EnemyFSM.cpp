// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/GameplayStatics.h>
#include "TPSPlayer.h"
#include "Enemy.h"
#include "TPSProject.h"	// PRINT_LOG ����ϱ� ���ؼ�
#include <Components/CapsuleComponent.h>	// ĸ�� ������Ʈ �ݸ��� ��Ȱ��ȭ�� ����
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

	// ���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	// StaticClass() �Լ��� �� �Լ��� ������ Ŭ����(ATPSPlayer)�� ��ȯ�ϰ�,
	// GetActorOfClass() �Լ��� ���� ������ ���� ���� Ŭ������ ���� ���͸� �ϳ� ã�´�.
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());	
	
	// �߰��� ���͸� ATPSPlayer�� ĳ�����Ͽ� target���� ����
	target = Cast<ATPSPlayer>(actor);

	// ���� ��ü ��������
	// GetOwner() �Լ��� ���� ���� ��ü �����ͼ� ĳ�����ϰ� me�� ����
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* �Ҵ� (ABP_Enemy �Ҵ�)
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());

	// AAIController �Ҵ�
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

// ��� ����
void UEnemyFSM::IdleState()
{
	// ���� �ð� ��ٷȴٰ� �̵� ���·� ��ȯ�ϰ� �ʹ�.
	// 1. �ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;	// ��Ÿ Ÿ���� ��� ������Ŵ

	// 2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > idleDelayTime)
	{
		// 3. �̵� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;

		// ��� �ð� �ʱ�ȭ
		currentTime = 0;

		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;

		// ���� ������ ��ġ �����ֱ�
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �̵� ����
void UEnemyFSM::MoveState()
{
	// Ÿ��(Player)������ �̵��ϰ� �Ѵ�.
	// �׷��ٰ� Ÿ���� ��������� ���� ���·� �����Ѵ�.
	
	// 1. Ÿ���� ������ ��������
	FVector destination = target->GetActorLocation();

	// 2. ������ �ʿ��ϴ�.
	FVector dir = destination - me->GetActorLocation();

	// 3. �� �������� �̵��Ѵ�.
	//me->AddMovementInput(dir.GetSafeNormal());
	//ai->MoveToLocation(destination);

	// ������̼� �ý��� �ν��Ͻ� ���
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	// ������ �� ã�� ��� ������ �˻�
	FPathFindingQuery query;
	FAIMoveRequest req;

	// ���������� ������ �� �ִ� ���� 3cm
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);

	// �� ã�⸦ ���� ���� ����
	ai->BuildPathfindingQuery(req, query);

	// �� ã�� ��� ��������
	FPathFindingResult r = ns->FindPathSync(query);

	// ������ ��ã�� ���� ���� Ȯ��
	if (r.Result == ENavigationQueryResult::Success)
	{
		// ��ã�� �����Ͽ� Ÿ�� ������ �̵�
		ai->MoveToLocation(destination);
	}
	
	else
	{
		// ��ã�� ���з� ���� ��ġ �̵�
		auto result = ai->MoveToLocation(randomPos);

		// ������ ��ġ ������ ���� ��,
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// ���ο� ���� ��ġ ��������
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}


	// Ÿ�ٰ� ����� ���� ���� ���·� ��ȯ�Ѵ�.
	// 1. Ÿ�ٰ��� �Ÿ��� ���� ���� �ȿ� ���´ٸ�
	if (dir.Size() < attackRange)	// Size() �Լ��� ���� dir�� ���̸� ��ȯ����
	{
		// �� ã�� ��� ����
		ai->StopMovement();

		// 2. ���� ���·� ��ȯ
		mState = EEnemyState::Attack;

		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;

		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		anim->bAttackPlay = true;

		// ���� ���� ��ȯ ��, ��� �ð��� �ٷ� �������� ó��
		currentTime = attackDelayTime;
	}
}

// ���� ����
void UEnemyFSM::AttackState()
{
	// ���� �ð����� �� �� �����Ѵ�.
	// 1. �ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 2. ���� ��� �ð��� �ʰ��ϸ�
	if (currentTime > attackDelayTime)
	{
		//// 3. �����Ѵ�.
		//PRINT_LOG(TEXT("Attack to Player"));

		// ��� �ð� �ʱ�ȭ
		currentTime = 0;

		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		anim->bAttackPlay = true;
	}

	// Ÿ���� ���� ������ ����� ���¸� �̵����� ��ȯ�Ѵ�.
	// 1. Ÿ�ٰ��� �Ÿ� ���ϱ�
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	// 2. Ÿ�ٰ��� �Ÿ��� ���� ������ �Ѿ�ٸ�
	if (distance > attackRange)
	{
		mState = EEnemyState::Move;
		
		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;

		// ���� ������ ��ġ �����ֱ�
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

// �ǰ� ����
void UEnemyFSM::DamageState() 
{
	// 1. �ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	// ���� ��� �ð��� �ǰ� ���ð��� �ʰ��ߴٸ�
	if (currentTime > damageDelayTime)
	{
		// 3. ��� ���·� ��ȯ
		mState = EEnemyState::Idle;

		// ��� �ð� �ʱ�ȭ
		currentTime = 0;

		// �ִϸ��̼� ���� ����ȭ
		anim->animState = mState;
	}
}

// ���� ����
void UEnemyFSM::DieState()
{
	// ���� ���� �ִϸ��̼��� ������ �ʾҴٸ�
	// �ٴ� �������� �ʵ��� ��.(�ٷ� return�ؼ� �Ʒ� ����δ� ���� X)
	if (anim->bDieDone == false)
	{
		return;
	}

	// 1. ��� �Ʒ��� ������
	// ��� � ���� P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;

	me->SetActorLocation(P);

	// 2. �������ٰ� ȭ�鿡�� �Ⱥ��̸�(= 2m ��������) �����Ѵ�.
	if (P.Z < -200.0f)
	{
		// �����Ѵ�.
		me->Destroy();
	}
}


// �ǰ� �˸� �̺�Ʈ
// TPSPlayer.cpp�� InputFire() ���� ���
void UEnemyFSM::OnDamageProcess()
{
	// ü�� 1�� ����
	hp--;	

	// ü���� ���� ������
	if (hp > 0)
	{
		// �ǰ� ���·� ��ȯ
		mState = EEnemyState::Damage;

		currentTime = 0;

		// �ǰ� �ִϸ��̼� ���
		int32 index = FMath::RandRange(0, 1);	// RandRange() �Լ��� 0�� 1�� ���� �� �ϳ� ����
		FString sectionName = FString::Printf(TEXT("Damage%d"), index);	// ��Ÿ���� �����̸� ����
		anim->PlayDamageAnim(FName(*sectionName));	// PlayDamgeAnim() �Լ��� ��Ÿ�� �����̸��� �����Ͽ�, �ִϸ��̼� ���
													// ������ ��, �Ű������� �ڷ����� FName�̹Ƿ� FName ������ ��ȯ�� ����
	}

	// ü���� ���ٸ�
	else
	{
		// ���� ���·� ��ȯ
		mState = EEnemyState::Die;

		// �ٴ����� �������� ��, ĸ�� �浹ü ��Ȱ��ȭ ������Ѵ�.
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// ���� �ִϸ��̼� ���
		anim->PlayDamageAnim(FName("Die"));
	}

	// �ִϸ��̼� ���� ����ȭ
	anim->animState = mState;
	// ������ ����
	ai->StopMovement();
}

// ���� ��ġ �������� �Լ�
bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());	// ������̼� �ý��� �ν��Ͻ��� GetNavigationSystme()�Լ��� ������
	FNavLocation loc;	// ��ġ ���� ���� ����
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);	// ������ ������̼� �ý��� �ν��Ͻ��� ��� �Լ� GetRandomReachablePointInRadius() ȣ��
																					// �Լ��� �־��� centerLocation �������� radius���� �ȿ� �������� ��ġ ������ �����ͼ�
																					// loc ������ ��ġ ������ �����

	dest = loc.Location;	// �̷��� ���� ���� ��ġ�� dest ������ �Ҵ�

	return result;	// ���������� ������ �����Դ��� true/false ��ȯ
}