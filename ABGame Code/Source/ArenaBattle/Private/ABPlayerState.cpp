// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABCharacter.h"
#include "ABSaveGame.h"


AABPlayerState::AABPlayerState()
{
	gameScore = 0;			// ���� ���� �ʱ�ȭ
	characterLevel = 1;		// ĳ���� ���� �ʱ�ȭ
	Exp = 0;				// ����ġ

	gameHighScore = 0;		// ���� �ְ� ���� �ʱ�ȭ
	characterIndex = 0;		// ĳ���� ���̷�Ż �޽� �ε��� �ʱ�ȭ
	// ���� ���̺� ��ɿ� �� ���� ���Ͽ� ������ �� �ִ� ���� ���� �̸�
	SaveSlotName = TEXT("Player1");		
}

// �÷��̾� ������Ʈ �ʱ�ȭ �Լ�
// �÷��̾� ��Ʈ�ѷ��� ������ �Ϸ��ϴ� ����(ABGameMode�� PostLogin()�Լ�)���� ȣ��
void AABPlayerState::InitPlayerData()
{
	// LoadGameFromSlot(���Ը�, ����� �ε���) �Լ��� �̿��Ͽ�, ������ ������ SaveGame ������Ʈ �ε�
	auto saveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (saveGame == nullptr)	// ���� ó�� �������� �ε��� SaveGame ������Ʈ�� ���ٸ�
	{
		saveGame = GetMutableDefault<UABSaveGame>();	// Ŭ������ ���� ������ �⺻ ��ü�� �����´�.
	}

	SetPlayerName(saveGame->playerName);			// ĳ���� �г��� �ʱ� ����
	SetCharacterLevel(saveGame->level);				// ĳ���� ���� �ʱ� ����
	gameScore = 0;									// ���� ���� �ʱ� ����
	gameHighScore = saveGame->highScore;			// ���� �ְ� ���� �ʱ� ����
	Exp = saveGame->exp;							// ����ġ 
	characterIndex = saveGame->CharacterIndex;		// ĳ���� ���̷�Ż �޽� �ε��� �ʱ� ����
	savePlayerData();								// ���� �������ڸ���, �÷��̾� ������ ����
}

// �÷��̾� �����͸� �����ϴ� �Լ�
void AABPlayerState::savePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();	// SaveGame ������Ʈ ����
	NewPlayerData->playerName = GetPlayerName();			// SaveGame�� �÷��̾� �̸� ����
	NewPlayerData->level = characterLevel;					// SaveGame�� �÷��̾� ���� ����
	NewPlayerData->exp = Exp;								// SaveGame�� �÷��̾� ����ġ ����
	NewPlayerData->highScore = gameHighScore;				// SaveGame�� �÷��̾� �ְ� ���� ����
	NewPlayerData->CharacterIndex = characterIndex;			// SaveGame�� �÷��̾� ĳ���� ���̷�Ż �޽� �ε��� ����

	// SaveGameObject�� �������� �÷����� �ش� ���Կ� ����
	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))	// ���� ���� ��,
	{
		ABLOG(Error, TEXT("SaveGame Error !"));
	}
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) return 0;	// ������ ���

	float Result = (float)Exp / (float)CurrentStatData->NextExp;	// ���� ����ġ / �������� �ʿ��� �ִ� ����ġ
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

// ������ �ߴ��� ���ߴ��� ��ȯ
bool AABPlayerState::AddExp(int32 InComeExp)
{
	if (CurrentStatData->NextExp == -1) return false;	// ������ ���

	bool DidLevelUp = false;
	Exp = Exp + InComeExp;

	if (Exp >= CurrentStatData->NextExp)	// ������
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(characterLevel + 1);	// ���� + 1 ��, �ش� ������ ������ ���̺� ���� ������
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();		// ����ġ�� ȹ�� �Ǵ� ������ ������, �÷��̾� ������Ʈ ���� ��������Ʈ�� ȣ��
	savePlayerData();						// ����ġ�� ������� ��, �÷��̾� ������ ����
	return DidLevelUp;
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto GI = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(GI != nullptr);
		
	// �������� ������ ������ ���̺� ���� ������
	CurrentStatData = GI->GetABChracterData(NewCharacterLevel);
	ABCHECK(CurrentStatData != nullptr);

	characterLevel = NewCharacterLevel;
}

void AABPlayerState::AddGameScore()
{
	gameScore++;						// ���� ���ھ� 1 ����

	if (gameScore >= gameHighScore)		// ���� ���� ���ھ ���� �ְ� ���ھ �ѱ��
	{
		gameHighScore = gameScore;		// ���� �ְ� ���ھ� = ���� ���� ���ھ�
	}

	// ���� ���ھ ����Ǿ����� OnPlayerStateChanged ��������Ʈ ȣ��(HUD Widget�� txtCurrentScore�� �����ϱ� ���ؼ�)
	OnPlayerStateChanged.Broadcast();
	savePlayerData();					// ������ �ٲ���� ��(), �÷��̾� ������ ����
}


int32 AABPlayerState::GetGameHighScore() const
{
	return gameHighScore;	// ���� �ְ� ���� ��ȯ
}

int32 AABPlayerState::GetGameScore() const
{
	return gameScore;	// ���� ���� ��ȯ
}


int32 AABPlayerState::GetCharacterLevel() const
{
	return characterLevel;	// ĳ���� ���� ��ȯ
}


int32 AABPlayerState::GetCharacterIndex() const
{
	return characterIndex;	// ĳ���� ���̷�Ż �޽� �ε��� ��ȯ
}
