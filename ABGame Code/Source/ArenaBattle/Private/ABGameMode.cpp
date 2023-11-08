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

	ScoreToClear = 5;		// 스코어 2를 획득해야 미션 클리어

	//static ConstructorHelpers::FClassFinder<APawn> BP_Quinn(TEXT("/Script/Engine.Blueprint'/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C'"));
	//
	//if (BP_Quinn.Succeeded()) {
	//	DefaultPawnClass = BP_Quinn.Class;
	//}
}

// 폰과 플레이어 컨트롤러가 생성되는 시점
void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

// 전달받은 플레이어 컨트롤러의 점수 추가
void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)	
{
	// 현재 게임에 참가한 플레이어 컨트롤러의 목록을 가져옴
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());	// 플레이어 컨트롤러를 가져옴
		
		if(ABPlayerController != nullptr && ScoredPlayer == ABPlayerController)	// NPC가 아닌 플레이어인지 ?
		{
			ABPlayerController->AddGameScore();	// 플레이어 스테이트에 있는 스코어 점수 증가 함수 호출
			break;
		}
	}
	ABGameState->AddGameScore();		// 게임 스테이트에 있는 스코어 점수 증가 함수 호출

	// 현재 점수가 미션을 깨는데 필요한 점수보다 높은지 ?
	if (GetScore() >= ScoreToClear)
	{
		ABGameState->SetGameCleared();	// 미션 클리어한 것으로 변경

		// 모든 폰을 정지시킨다. 
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		// 현재 게임에 참여중인 플레이어 컨트롤러의 목록을 월드에서 제공하는 GetPlayerControllerIterator을 사용하여
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			// 플레이어 컨트롤러를 찾아낸다.
			const auto playerController = Cast<AABPlayerController>(It->Get());
			if (playerController != nullptr)
			{
				playerController->ShowResultUI();	// 결과 UI 띄우기
			}
		}
	}
}

// 게임 스코어 반환
int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}

// 플레이어 컨트롤러 구성을 완료하는 시점
void AABGameMode::PostLogin(APlayerController* NewPlayer) 
{
	ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);

	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);	// 컨트롤러가 가지고있는 플레이어 스테이트 가져오기
	ABCHECK(ABPlayerState != nullptr);
	ABPlayerState->InitPlayerData();	// 플레이어 컨트롤러 구성을 완료하는 시점에 PlayerState도 초기화
}