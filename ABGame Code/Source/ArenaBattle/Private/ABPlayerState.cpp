// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABCharacter.h"
#include "ABSaveGame.h"


AABPlayerState::AABPlayerState()
{
	gameScore = 0;			// 게임 점수 초기화
	characterLevel = 1;		// 캐릭터 레벨 초기화
	Exp = 0;				// 경험치

	gameHighScore = 0;		// 게임 최고 점수 초기화
	characterIndex = 0;		// 캐릭터 스켈레탈 메시 인덱스 초기화
	// 게임 세이브 기능에 각 저장 파일에 접근할 수 있는 고유 슬롯 이름
	SaveSlotName = TEXT("Player1");		
}

// 플레이어 스테이트 초기화 함수
// 플레이어 컨트롤러의 구성을 완료하는 시점(ABGameMode의 PostLogin()함수)에서 호출
void AABPlayerState::InitPlayerData()
{
	// LoadGameFromSlot(슬롯명, 사용자 인덱스) 함수를 이용하여, 지정된 슬롯의 SaveGame 오브젝트 로드
	auto saveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (saveGame == nullptr)	// 게임 처음 시작으로 로드할 SaveGame 오브젝트가 없다면
	{
		saveGame = GetMutableDefault<UABSaveGame>();	// 클래스의 변경 가능한 기본 개체를 가져온다.
	}

	SetPlayerName(saveGame->playerName);			// 캐릭터 닉네임 초기 설정
	SetCharacterLevel(saveGame->level);				// 캐릭터 레벨 초기 설정
	gameScore = 0;									// 게임 점수 초기 설정
	gameHighScore = saveGame->highScore;			// 게임 최고 점수 초기 설정
	Exp = saveGame->exp;							// 경험치 
	characterIndex = saveGame->CharacterIndex;		// 캐릭터 스켈레탈 메시 인덱스 초기 설정
	savePlayerData();								// 게임 시작하자마자, 플레이어 데이터 저장
}

// 플레이어 데이터를 저장하는 함수
void AABPlayerState::savePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();	// SaveGame 오브젝트 생성
	NewPlayerData->playerName = GetPlayerName();			// SaveGame에 플레이어 이름 저장
	NewPlayerData->level = characterLevel;					// SaveGame에 플레이어 레벨 저장
	NewPlayerData->exp = Exp;								// SaveGame에 플레이어 경험치 저장
	NewPlayerData->highScore = gameHighScore;				// SaveGame에 플레이어 최고 점수 저장
	NewPlayerData->CharacterIndex = characterIndex;			// SaveGame에 플레이어 캐릭터 스켈레탈 메시 인덱스 저장

	// SaveGameObject의 콘텐츠를 플랫폼별 해당 슬롯에 저장
	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))	// 저장 실패 시,
	{
		ABLOG(Error, TEXT("SaveGame Error !"));
	}
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) return 0;	// 만렙일 경우

	float Result = (float)Exp / (float)CurrentStatData->NextExp;	// 현재 경험치 / 레벨업에 필요한 최대 경험치
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

// 레벨업 했는지 안했는지 반환
bool AABPlayerState::AddExp(int32 InComeExp)
{
	if (CurrentStatData->NextExp == -1) return false;	// 만렙일 경우

	bool DidLevelUp = false;
	Exp = Exp + InComeExp;

	if (Exp >= CurrentStatData->NextExp)	// 레벨업
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(characterLevel + 1);	// 레벨 + 1 후, 해당 레벨의 데이터 테이블 행을 가져옴
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();		// 경험치를 획득 또는 레벨업 했으니, 플레이어 스테이트 변경 델리게이트를 호출
	savePlayerData();						// 경혐치가 변경됐을 때, 플레이어 데이터 저장
	return DidLevelUp;
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto GI = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(GI != nullptr);
		
	// 레벨업된 레벨로 데이터 테이블 행을 가져옴
	CurrentStatData = GI->GetABChracterData(NewCharacterLevel);
	ABCHECK(CurrentStatData != nullptr);

	characterLevel = NewCharacterLevel;
}

void AABPlayerState::AddGameScore()
{
	gameScore++;						// 게임 스코어 1 증가

	if (gameScore >= gameHighScore)		// 현재 게임 스코어가 게임 최고 스코어를 넘기면
	{
		gameHighScore = gameScore;		// 게임 최고 스코어 = 현재 게임 스코어
	}

	// 게임 스코어가 변경되었으니 OnPlayerStateChanged 델리게이트 호출(HUD Widget의 txtCurrentScore를 수정하기 위해서)
	OnPlayerStateChanged.Broadcast();
	savePlayerData();					// 점수가 바뀌었을 때(), 플레이어 데이터 저장
}


int32 AABPlayerState::GetGameHighScore() const
{
	return gameHighScore;	// 게임 최고 점수 반환
}

int32 AABPlayerState::GetGameScore() const
{
	return gameScore;	// 게임 점수 반환
}


int32 AABPlayerState::GetCharacterLevel() const
{
	return characterLevel;	// 캐릭터 레벨 반환
}


int32 AABPlayerState::GetCharacterIndex() const
{
	return characterIndex;	// 캐릭터 스켈레탈 메시 인덱스 반환
}
