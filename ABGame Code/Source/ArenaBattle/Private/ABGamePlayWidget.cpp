// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGamePlayWidget.h"
#include "Components/Button.h"
#include "ABPlayerController.h"


// UI 초기화 되는 시점에 호출되는 함수
void UABGamePlayWidget::NativeConstruct()
{
	Super::NativeConstruct();


	// 게임으로 돌아가기 버튼 바인딩
	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (ResumeButton != nullptr)
	{
		// 게임으로 돌아가기 버튼 클릭 시, OnResumeClicked() 함수 호출
		ResumeButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnResumeClicked);
	}

	// 타이틀화면으로 버튼 바인딩
	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (ReturnToTitleButton != nullptr)
	{
		// 타이틀화면으로 버튼 클릭 시, OnReturnToTitleClicked() 함수 호출
		ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnReturnToTitleClicked);
	}

	// 재도전 버튼 바인딩
	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (RetryGameButton != nullptr)
	{
		// 재도전 버튼 클릭 시, OnRetryGameClicked() 함수 호출
		RetryGameButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnRetryGameClicked);
	}
}

// 게임으로 돌아가기 버튼 클릭 시 ~
void UABGamePlayWidget::OnResumeClicked()
{
	// 현재 자신을 생성하고, 관리하는 플레이어 컨트롤러 정보가져오기
	auto playerController = Cast<AABPlayerController>(GetOwningPlayer());	
	ABCHECK(playerController != nullptr);

	RemoveFromParent();		// 현재 띄워진 자신의 UI 제거	

	playerController->ChangeInputMode(true);	// 게임에만 입력되도록 + 마우스 커서 안보이도록
	playerController->SetPause(false);			// 일시정지 해제
}

// 타이틀 화면으로 버튼 클릭 시 ~
void UABGamePlayWidget::OnReturnToTitleClicked()
{
	// 타이틀 레벨 열기
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

// 재도전 버튼 클릭 시 ~
void UABGamePlayWidget::OnRetryGameClicked()
{
	// 현재 자신을 생성하고, 관리하는 플레이어 컨트롤러 정보가져오기
	auto playerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(playerController != nullptr);

	// 현재 레벨(= GamePlay 레벨)을 재시작
	playerController->RestartLevel();
}