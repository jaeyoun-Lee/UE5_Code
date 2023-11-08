// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AABGameMode();

	virtual void PostInitializeComponents() override;		// ���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ����
	virtual void PostLogin(APlayerController* NewPlayer) override;		// �÷��̾� ��Ʈ�ѷ� ������ �Ϸ��ϴ� ����
	void AddScore(class AABPlayerController* ScoredPlayer);	// ���޹��� �÷��̾� ��Ʈ�ѷ��� ���� ����

	int32 GetScore() const;			// ���� ���� ���ھ� ��������

private:
	UPROPERTY()
	class AABGameState* ABGameState;	// ���� ������Ʈ�� ������ ����

	UPROPERTY()
	int32 ScoreToClear;				// �̼� ���� ���θ� ���� ȹ���ؾ��ϴ� ���� ��
};
