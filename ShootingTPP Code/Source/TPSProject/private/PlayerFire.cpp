// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include "EnemyFSM.h"
#include "TPSProject.h"
#include "PlayerAnim.h"
#include "Bullet.h"


// 생성자
UPlayerFire::UPlayerFire()
{
	// 총알 사운드 가져오기
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	
	// 총알 사운드를 잘 가져왔다면,
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}

void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();

	tpsCamComp = me->tpsCamComp;
	gunMeshComp = me->gunMeshComp;
	sniperGunComp = me->sniperGunComp;

	// 기본으로 스나이퍼 총 사용하도록함.
	ChangeToSniper();

	// 1. 스나이퍼 모드 UI 위젯 인스턴스 생성
	sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. 스나이퍼 일반 조준 모드 UI 위젯 인스턴스 생성
	crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. 일반 조준 UI가 보이게
	crosshairUI->AddToViewport();

}


void UPlayerFire::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	// 총알 발사 이벤트 처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerFire::InputFire);

	// 총 교체 이벤트 처리 함수 바인딩
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UPlayerFire::ChangeToSniper);

	// 스나이퍼 조준 모드 이벤트 처리 함수 바인딩
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Pressed, this, &UPlayerFire::SniperAim);
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Released, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::InputFire()
{
	// 공격 애니메이션 재생
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// 총 발사시, 총 사운드 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// 1번 유탄총 사용 시,
	if (bUsingGrenadeGun)
	{
		// 총알 발사 처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}

	// 2번 스나이퍼 총 사용 시,
	else
	{
		// LineTrace의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();

		// LineTrace의 종료 위치(카메라가 바라보는 방향으로 5000cm 떨어진 지점)
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;

		// LineTrace의 충돌 정보를 담을 변수
		FHitResult hitInfo;

		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;

		// 자기 자신(TPSPlayer)는 충돌에서 제외
		params.AddIgnoredActor(me);

		// Channel필터를 이용한 LineTrace 충돌 검출(충돌 정보, 시작위치, 종료위치, 검출 채널, 충돌옵션)
		// 비어있는 매개변수(= hitInfo)를 전달하면 충돌된게 어떤 것인지 담아서 돌려주는 거 같음
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// LineTrace가 부딪혔을 때
		if (bHit)
		{
			// 충돌 처리 -> 총알 파편 효과 재생
			// 먼저 효과가 놓일 위치, 회전, 크기 정보를 FTransform타입의 변수에 넣어야한다.
			FTransform bulletTrans;

			// 부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);

			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// 부딪힌 물체의 컴포넌트에 물리가 적용되어 있으면 날려버린다.
			// 부딪힐 물체 저장
			auto hitComp = hitInfo.GetComponent();

			// 1. 만약 부딪힌 컴포넌트에 물리가 적용되어있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 2. 날려버릴 힘과 방향 구하기(ImpactNormal은 부딪힌 지점의 면(총알이 날라온 방향을 반대로 가리키는 벡터)으로 ex) 총알이 -> 이케 날라가면 <- 이렇게 가리킴
				// -를 붙임으로서 총알이 날라가는 방향으로 날라가게 함.)
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;

				// 3. 그 방향으로 날리기
				hitComp->AddForce(force);
			}

			// 부딪힌 대상이 Enemy인지 확인
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));

			if (enemy)
			{
				PRINT_LOG(TEXT("Hit to Enemy"));
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

// 유탄총으로 변경
void UPlayerFire::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);

	// 유탄 총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

// 스나이퍼로 변경
void UPlayerFire::ChangeToSniper()
{
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);

	// 유탄 총 사용할지 여부 전달
	me->OnUsingGrenade(bUsingGrenadeGun);
}

// 스나이퍼 조준 함수
void UPlayerFire::SniperAim()
{
	// 스나이퍼 총 모드가 아니라면 처리하지 않는다.
	if (bUsingGrenadeGun)
	{
		return;
	}

	// Pressed입력 처리
	if (bSniperMode == false)
	{
		// 1. 스나이퍼 조준 모드 활성화
		bSniperMode = true;

		// 2. 스나이퍼 조준 UI 등록
		sniperUI->AddToViewport();

		// 3. 카메라의 시야각 Field Of View 설정(확대)
		tpsCamComp->SetFieldOfView(45);

		// 4. 일반 조준 UI 제거
		crosshairUI->RemoveFromParent();
	}

	// Release 입력처리(Ctrl키를 뗄 때)
	else
	{
		// 1. 스나이퍼 조준 모드 비활성화
		bSniperMode = false;

		// 2. 스나이퍼 조준 UI 화면에서 제거
		sniperUI->RemoveFromParent();

		// 3. 카메라의 시야각 원래대로 복구
		tpsCamComp->SetFieldOfView(90.0f);

		// 4. 일반 조준 UI 생성
		crosshairUI->AddToViewport();
	}
}
