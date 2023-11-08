// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include "EnemyFSM.h"
#include "TPSProject.h"
#include "PlayerAnim.h"
#include "Bullet.h"


// ������
UPlayerFire::UPlayerFire()
{
	// �Ѿ� ���� ��������
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	
	// �Ѿ� ���带 �� �����Դٸ�,
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

	// �⺻���� �������� �� ����ϵ�����.
	ChangeToSniper();

	// 1. �������� ��� UI ���� �ν��Ͻ� ����
	sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. �������� �Ϲ� ���� ��� UI ���� �ν��Ͻ� ����
	crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. �Ϲ� ���� UI�� ���̰�
	crosshairUI->AddToViewport();

}


void UPlayerFire::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	// �Ѿ� �߻� �̺�Ʈ ó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerFire::InputFire);

	// �� ��ü �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UPlayerFire::ChangeToGrenadeGun);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UPlayerFire::ChangeToSniper);

	// �������� ���� ��� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Pressed, this, &UPlayerFire::SniperAim);
	PlayerInputComponent->BindAction(TEXT("SniperMode"), IE_Released, this, &UPlayerFire::SniperAim);
}

void UPlayerFire::InputFire()
{
	// ���� �ִϸ��̼� ���
	auto anim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// ī�޶� ����ũ ���
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	// �� �߻��, �� ���� ���
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	// 1�� ��ź�� ��� ��,
	if (bUsingGrenadeGun)
	{
		// �Ѿ� �߻� ó��
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}

	// 2�� �������� �� ��� ��,
	else
	{
		// LineTrace�� ���� ��ġ
		FVector startPos = tpsCamComp->GetComponentLocation();

		// LineTrace�� ���� ��ġ(ī�޶� �ٶ󺸴� �������� 5000cm ������ ����)
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;

		// LineTrace�� �浹 ������ ���� ����
		FHitResult hitInfo;

		// �浹 �ɼ� ���� ����
		FCollisionQueryParams params;

		// �ڱ� �ڽ�(TPSPlayer)�� �浹���� ����
		params.AddIgnoredActor(me);

		// Channel���͸� �̿��� LineTrace �浹 ����(�浹 ����, ������ġ, ������ġ, ���� ä��, �浹�ɼ�)
		// ����ִ� �Ű�����(= hitInfo)�� �����ϸ� �浹�Ȱ� � ������ ��Ƽ� �����ִ� �� ����
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// LineTrace�� �ε����� ��
		if (bHit)
		{
			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
			// ���� ȿ���� ���� ��ġ, ȸ��, ũ�� ������ FTransformŸ���� ������ �־���Ѵ�.
			FTransform bulletTrans;

			// �ε��� ��ġ �Ҵ�
			bulletTrans.SetLocation(hitInfo.ImpactPoint);

			// �Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// �ε��� ��ü�� ������Ʈ�� ������ ����Ǿ� ������ ����������.
			// �ε��� ��ü ����
			auto hitComp = hitInfo.GetComponent();

			// 1. ���� �ε��� ������Ʈ�� ������ ����Ǿ��ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				// 2. �������� ���� ���� ���ϱ�(ImpactNormal�� �ε��� ������ ��(�Ѿ��� ����� ������ �ݴ�� ����Ű�� ����)���� ex) �Ѿ��� -> ���� ���󰡸� <- �̷��� ����Ŵ
				// -�� �������μ� �Ѿ��� ���󰡴� �������� ���󰡰� ��.)
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;

				// 3. �� �������� ������
				hitComp->AddForce(force);
			}

			// �ε��� ����� Enemy���� Ȯ��
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

// ��ź������ ����
void UPlayerFire::ChangeToGrenadeGun()
{
	bUsingGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);

	// ��ź �� ������� ���� ����
	me->OnUsingGrenade(bUsingGrenadeGun);
}

// �������۷� ����
void UPlayerFire::ChangeToSniper()
{
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);

	// ��ź �� ������� ���� ����
	me->OnUsingGrenade(bUsingGrenadeGun);
}

// �������� ���� �Լ�
void UPlayerFire::SniperAim()
{
	// �������� �� ��尡 �ƴ϶�� ó������ �ʴ´�.
	if (bUsingGrenadeGun)
	{
		return;
	}

	// Pressed�Է� ó��
	if (bSniperMode == false)
	{
		// 1. �������� ���� ��� Ȱ��ȭ
		bSniperMode = true;

		// 2. �������� ���� UI ���
		sniperUI->AddToViewport();

		// 3. ī�޶��� �þ߰� Field Of View ����(Ȯ��)
		tpsCamComp->SetFieldOfView(45);

		// 4. �Ϲ� ���� UI ����
		crosshairUI->RemoveFromParent();
	}

	// Release �Է�ó��(CtrlŰ�� �� ��)
	else
	{
		// 1. �������� ���� ��� ��Ȱ��ȭ
		bSniperMode = false;

		// 2. �������� ���� UI ȭ�鿡�� ����
		sniperUI->RemoveFromParent();

		// 3. ī�޶��� �þ߰� ������� ����
		tpsCamComp->SetFieldOfView(90.0f);

		// 4. �Ϲ� ���� UI ����
		crosshairUI->AddToViewport();
	}
}
