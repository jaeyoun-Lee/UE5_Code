// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGamePlayWidget.h"
#include "Components/Button.h"
#include "ABPlayerController.h"


// UI �ʱ�ȭ �Ǵ� ������ ȣ��Ǵ� �Լ�
void UABGamePlayWidget::NativeConstruct()
{
	Super::NativeConstruct();


	// �������� ���ư��� ��ư ���ε�
	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (ResumeButton != nullptr)
	{
		// �������� ���ư��� ��ư Ŭ�� ��, OnResumeClicked() �Լ� ȣ��
		ResumeButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnResumeClicked);
	}

	// Ÿ��Ʋȭ������ ��ư ���ε�
	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (ReturnToTitleButton != nullptr)
	{
		// Ÿ��Ʋȭ������ ��ư Ŭ�� ��, OnReturnToTitleClicked() �Լ� ȣ��
		ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnReturnToTitleClicked);
	}

	// �絵�� ��ư ���ε�
	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (RetryGameButton != nullptr)
	{
		// �絵�� ��ư Ŭ�� ��, OnRetryGameClicked() �Լ� ȣ��
		RetryGameButton->OnClicked.AddDynamic(this, &UABGamePlayWidget::OnRetryGameClicked);
	}
}

// �������� ���ư��� ��ư Ŭ�� �� ~
void UABGamePlayWidget::OnResumeClicked()
{
	// ���� �ڽ��� �����ϰ�, �����ϴ� �÷��̾� ��Ʈ�ѷ� ������������
	auto playerController = Cast<AABPlayerController>(GetOwningPlayer());	
	ABCHECK(playerController != nullptr);

	RemoveFromParent();		// ���� ����� �ڽ��� UI ����	

	playerController->ChangeInputMode(true);	// ���ӿ��� �Էµǵ��� + ���콺 Ŀ�� �Ⱥ��̵���
	playerController->SetPause(false);			// �Ͻ����� ����
}

// Ÿ��Ʋ ȭ������ ��ư Ŭ�� �� ~
void UABGamePlayWidget::OnReturnToTitleClicked()
{
	// Ÿ��Ʋ ���� ����
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

// �絵�� ��ư Ŭ�� �� ~
void UABGamePlayWidget::OnRetryGameClicked()
{
	// ���� �ڽ��� �����ϰ�, �����ϴ� �÷��̾� ��Ʈ�ѷ� ������������
	auto playerController = Cast<AABPlayerController>(GetOwningPlayer());
	ABCHECK(playerController != nullptr);

	// ���� ����(= GamePlay ����)�� �����
	playerController->RestartLevel();
}