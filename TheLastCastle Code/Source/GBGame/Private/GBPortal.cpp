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
		// 시간을 꾸준히 더하며 몇초간 머물렀는지 계산하는 함수 CheckTimer를 반복 실행
		GetWorld()->GetTimerManager().SetTimer(stayTimer, this, &AGBPortal::CheckTimer, 0.1f, true);
		GBLOG(Warning, TEXT("Start Overlap"));
	}
}

void AGBPortal::TimerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AGBCharacter>(OtherActor))
	{
		// 포탈에서 떨어지면 머물렀던 시간 초기화 및 타이머 제거
		GetWorld()->GetTimerManager().ClearTimer(stayTimer);
		currentTime = 0;

		GBLOG(Warning, TEXT("End Overlap"));
	}
}

void AGBPortal::CheckTimer()
{
	currentTime += 0.1f;

	// 다음 레벨로 넘어가는 포탈을 2초이상 밟고 있으면
	if (currentTime >= 2.0f)
	{
		GBLOG(Warning, TEXT("Stay 2 seconds..."));

		auto playerControl = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (playerControl != nullptr)
		{
			playerControl->ShowNextUI();				// 화면이 흐릿해지는 UI 생성
			playerControl->bShowMouseCursor = false;	// 마우스 커서 X

			// 다음 레벨로 넘어갈때 사용하는 UI 위젯의 애니메이션을 가져오기 위해 위젯을 가져옴
			nextLVWidget = Cast<UNextLevelWidget>(playerControl->GetNextLevelWidget());
			if (nextLVWidget != nullptr)
			{
				// *** 중요 ***
				// 위젯의 애니메이션을 가져옴(애니메이션은 위젯의 부모 클래스에서 meta = (BindWidgetAnim)을 사용해서 가져올 수 있음)
				UIAnimation = nextLVWidget->GetWidgetAnimation();
				if (UIAnimation != nullptr)
				{
					// 다음 레벨로 넘어가는 UI에서 애니메이션 재생(화면 흐릿 + 어둡게)
					nextLVWidget->PlayAnimation(UIAnimation);
				}
			}
		}

		// 화면이 어둡게 되고 1초 후, 다음 레벨로 넘어가는 NextLevelOpen() 함수 실행
		GetWorld()->GetTimerManager().SetTimer(nextLevelTimer, this, &AGBPortal::NextLevelOpen, 1.0f, false, 1.5f);

		// 사용했던 머무른 시간을 계산했던 타이머 제거
		GetWorld()->GetTimerManager().ClearTimer(stayTimer);
		currentTime = 0;
	}
}

void AGBPortal::NextLevelOpen()
{
	// 사용했던 레벨을 여는 타이머 제거
	GetWorld()->GetTimerManager().ClearTimer(nextLevelTimer);

	// 현재 레벨이 OutSide 레벨이면 Inside 레벨로 이동
	if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleOutSide")))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("CastleInSide"));
	}
	
	// 현재 레벨이 InSide 레벨이면 BossStage 레벨로 이동
	else if (UGameplayStatics::GetCurrentLevelName(this).Contains(TEXT("CastleInSide")))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("BossStage"));
	}
}