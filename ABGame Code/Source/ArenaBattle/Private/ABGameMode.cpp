// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
//#include "ABPawn.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode() 
{
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameState::StaticClass();

	ScoreToClear = 5;		// ���ھ� 2�� ȹ���ؾ� �̼� Ŭ����

	//static ConstructorHelpers::FClassFinder<APawn> BP_Quinn(TEXT("/Script/Engine.Blueprint'/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C'"));
	//
	//if (BP_Quinn.Succeeded()) {
	//	DefaultPawnClass = BP_Quinn.Class;
	//}
}

// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

// ���޹��� �÷��̾� ��Ʈ�ѷ��� ���� �߰�
void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)	
{
	// ���� ���ӿ� ������ �÷��̾� ��Ʈ�ѷ��� ����� ������
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());	// �÷��̾� ��Ʈ�ѷ��� ������
		
		if(ABPlayerController != nullptr && ScoredPlayer == ABPlayerController)	// NPC�� �ƴ� �÷��̾����� ?
		{
			ABPlayerController->AddGameScore();	// �÷��̾� ������Ʈ�� �ִ� ���ھ� ���� ���� �Լ� ȣ��
			break;
		}
	}
	ABGameState->AddGameScore();		// ���� ������Ʈ�� �ִ� ���ھ� ���� ���� �Լ� ȣ��

	// ���� ������ �̼��� ���µ� �ʿ��� �������� ������ ?
	if (GetScore() >= ScoreToClear)
	{
		ABGameState->SetGameCleared();	// �̼� Ŭ������ ������ ����

		// ��� ���� ������Ų��. 
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		// ���� ���ӿ� �������� �÷��̾� ��Ʈ�ѷ��� ����� ���忡�� �����ϴ� GetPlayerControllerIterator�� ����Ͽ�
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			// �÷��̾� ��Ʈ�ѷ��� ã�Ƴ���.
			const auto playerController = Cast<AABPlayerController>(It->Get());
			if (playerController != nullptr)
			{
				playerController->ShowResultUI();	// ��� UI ����
			}
		}
	}
}

// ���� ���ھ� ��ȯ
int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}

// �÷��̾� ��Ʈ�ѷ� ������ �Ϸ��ϴ� ����
void AABGameMode::PostLogin(APlayerController* NewPlayer) 
{
	ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);

	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);	// ��Ʈ�ѷ��� �������ִ� �÷��̾� ������Ʈ ��������
	ABCHECK(ABPlayerState != nullptr);
	ABPlayerState->InitPlayerData();	// �÷��̾� ��Ʈ�ѷ� ������ �Ϸ��ϴ� ������ PlayerState�� �ʱ�ȭ
}