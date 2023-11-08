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
	AABPlayerState();	// ������

	int32 GetGameScore() const;			// ���� ���� �������� �Լ�	
	int32 GetCharacterLevel() const;	// ĳ���� ���� �������� �Լ�
	float GetExpRatio() const;			// ���� ����ġ ���� ���� ��������
	int32 GetCharacterIndex() const;	// ���� ĳ���� ���̷�Ż �޽� �ε��� ��������
	bool AddExp(int32 IncomeExp);		// ����ġ ������� Ȯ��
	void AddGameScore();				// ���� ���ھ� ����

	void InitPlayerData();				// �÷��̾� ������Ʈ �ʱ�ȭ �Լ�
	void savePlayerData();				// �÷��̾� �����͸� �����ϴ� �Լ�

	int32 GetGameHighScore() const;		// ���� �ְ� ���� ��������
	FString SaveSlotName;				// ���� ���Ͽ� ������ �� �ִ� ���� ���� �̸� 


	FOnPlayerStateChangeDelegate OnPlayerStateChanged;

protected:
	
	UPROPERTY(Transient)
	int32 gameScore;		// ���� ���� ������ ����

	UPROPERTY(Transient)
	int32 characterLevel;	// ĳ���� ���� ������ ����

	UPROPERTY()
	int32 Exp;		// ���� ������ ����ġ

	UPROPERTY(Transient)
	int32 gameHighScore;	// ���� �ְ� ���� ������ ����

	UPROPERTY(Transient)
	int32 characterIndex;	// ĳ���� ���̷�Ż �޽� �ε���

private:
	void SetCharacterLevel(int32 NewCharacterLevel);	// ĳ���� ���� ����
	struct FABCharacterData* CurrentStatData;			// ���̺� �����Ϳ��� �ش� �� ���� ����
};
