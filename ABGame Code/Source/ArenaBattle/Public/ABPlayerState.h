// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangeDelegate);
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AABPlayerState();	// 생성자

	int32 GetGameScore() const;			// 게임 점수 가져오는 함수	
	int32 GetCharacterLevel() const;	// 캐릭터 레벨 가져오는 함수
	float GetExpRatio() const;			// 현재 경험치 비율 정보 가져오기
	int32 GetCharacterIndex() const;	// 현재 캐릭터 스켈레탈 메시 인덱스 가져오기
	bool AddExp(int32 IncomeExp);		// 경험치 얻었는지 확인
	void AddGameScore();				// 게임 스코어 증가

	void InitPlayerData();				// 플레이어 스테이트 초기화 함수
	void savePlayerData();				// 플레이어 데이터를 저장하는 함수

	int32 GetGameHighScore() const;		// 게임 최고 점수 가져오기
	FString SaveSlotName;				// 저장 파일에 접근할 수 있는 고유 슬롯 이름 


	FOnPlayerStateChangeDelegate OnPlayerStateChanged;

protected:
	
	UPROPERTY(Transient)
	int32 gameScore;		// 게임 점수 저장할 변수

	UPROPERTY(Transient)
	int32 characterLevel;	// 캐릭터 레벨 저장할 변수

	UPROPERTY()
	int32 Exp;		// 현재 축적된 경험치

	UPROPERTY(Transient)
	int32 gameHighScore;	// 게임 최고 점수 저장할 변수

	UPROPERTY(Transient)
	int32 characterIndex;	// 캐릭터 스켈레탈 메시 인덱스

private:
	void SetCharacterLevel(int32 NewCharacterLevel);	// 캐릭터 레벨 설정
	struct FABCharacterData* CurrentStatData;			// 테이블 데이터에서 해당 행 정보 저장
};
