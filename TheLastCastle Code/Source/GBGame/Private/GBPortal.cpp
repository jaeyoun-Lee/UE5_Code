// Fill out your copyright notice in the Description page of Project Settings.


#include "GBPortal.h"
#include "GBCharacter.h"
#include "GBPlayerController.h"
#include "NextLevelWidget.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGBPortal::AGBPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	portal = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Portal"));
	portalInside = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Portal Inner"));

	RootComponent = sceneComponent;
	collision->SetupAttachment(RootComponent);
	portal->SetupAttachment(RootComponent);
	portalInside->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> portalFX (TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_healAura.P_ky_healAura'"));
	if (portalFX.Succeeded())
	{
		portal->SetTemplate(portalFX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> portalInnerFX(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_waterBall.P_ky_waterBall'"));
	if (portalInnerFX.Succeeded())
	{
		portalInside->SetTemplate(portalInnerFX.Object);
	}

	collision->SetRelativeLocation(FVector(0, 0, 58.72f));
	collision->SetCapsuleHalfHeight(156.877609f);
	collision->SetCapsuleRadius(107.133354f);
	portalInside->SetRelativeLocation(FVector(0, 0, 59.78339f));
	portalInside->SetRelativeScale3D(FVector(1.7375f, 1.7375f, 0.323605f));

	currentTime = 0.0f;
	UIAnimation = nullptr;
}

// Called when the game starts or when spawned
void AGBPortal::BeginPlay()
{
	Super::BeginPlay();
	
	collision->OnComponentBeginOverlap.AddDynamic(this, &AGBPortal::TimerStart);
	collision->OnComponentEndOverlap.AddDynamic(this, &AGBPortal::TimerStop);
}

// Called every frame
void AGBPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGBPortal::TimerStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AGBCharacter>(OtherActor))
	{
		// �ð��� ������ ���ϸ� ���ʰ� �ӹ������� ����ϴ� �Լ� CheckTimer�� �ݺ� ����
		GetWorld()->GetTimerManager().SetTimer(stayTimer, this, &AGBPortal::CheckTimer, 0.1f, true);
		GBLOG(Warning, TEXT("Start Overlap"));
	}
}

void AGBPortal::TimerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AGBCharacter>(OtherActor))
	{
		// ��Ż���� �������� �ӹ����� �ð� �ʱ�ȭ �� Ÿ�̸� ����
		GetWorld()->GetTimerManager().ClearTimer(stayTimer);
		currentTime = 0;

		GBLOG(Warning, TEXT("End Overlap"));
	}
}

void AGBPortal::CheckTimer()
{
	currentTime += 0.1f;

	// ���� ������ �Ѿ�� ��Ż�� 2���̻� ��� ������
	if (currentTime >= 2.0f)
	{
		GBLOG(Warning, TEXT("Stay 2 seconds..."));

		auto playerControl = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (playerControl != nullptr)
		{
			playerControl->ShowNextUI();				// ȭ���� �帴������ UI ����
			playerControl->bShowMouseCursor = false;	// ���콺 Ŀ�� X

			// ���� ������ �Ѿ�� ����ϴ� UI ������ �ִϸ��̼��� �������� ���� ������ ������
			nextLVWidget = Cast<UNextLevelWidget>(playerControl->GetNextLevelWidget());
			if (nextLVWidget != nullptr)
			{
				// *** �߿� ***
				// ������ �ִϸ��̼��� ������(�ִϸ��̼��� ������ �θ� Ŭ�������� meta = (BindWidgetAnim)�� ����ؼ� ������ �� ����)
				UIAnimation = nextLVWidget->GetWidgetAnimation();
				if (UIAnimation != nullptr)
				{
					// ���� ������ �Ѿ�� UI���� �ִϸ��̼� ���(ȭ�� �帴 + ��Ӱ�)
					nextLVWidget->PlayAnimation(UIAnimation);
				}
			}
		}

		// ȭ���� ��Ӱ� �ǰ� 1�� ��, ���� ������ �Ѿ�� NextLevelOpen() �Լ� ����
		GetWorld()->GetTimerManager().SetTimer(nextLevelTimer, this, &AGBPortal::NextLevelOpen, 1.0f, false, 1.5f);

		// ����ߴ� �ӹ��� �ð��� ����ߴ� Ÿ�̸� ����
		GetWorld()->GetTimerManager().ClearTimer(stayTimer);
		currentTime = 0;
	}
}

void AGBPortal::NextLevelOpen()
{
	// ����ߴ� ������ ���� Ÿ�̸� ����
	GetWorld()->GetTimerManager().ClearTimer(nextLevelTimer);

	// ���� ������ OutSide �����̸� Inside ������ �̵�
	if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleOutSide")))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("CastleInSide"));
	}
	
	// ���� ������ InSide �����̸� BossStage ������ �̵�
	else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleInSide")))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("BossStage"));
	}
}