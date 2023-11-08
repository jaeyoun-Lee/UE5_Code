// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GBGame.h"
#include "GameFramework/GameStateBase.h"
#include "GBGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameClearDelegate);
/**
 * 
 */
UCLASS()
class GBGAME_API AGBGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGBGameState();				// ������

	int32 RemainingEnemy();		// ���� ���� ��, Enemy�� ����̳� �ִ��� ī��Ʈ�ϴ� �Լ�
	int32 GetEnemyCount();		// ���� �����ִ� Enemy �� ��ȯ

	UFUNCTION(BlueprintCallable)
	void killedEnemy();			// Enemy�� ������ ����Ǵ� �Լ�

	FOnGameClearDelegate OnGameClear;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy, Meta = (AllowPrivateAccess = true))
	int32 enemyCount;			// ���� Enemy ��

	UPROPERTY(EditAnywhere, Category = ProtalLocation, Meta = (AllowPrivateAccess = true));
	FVector stage1_Location;

	UPROPERTY(EditAnywhere, Category = ProtalLocation, Meta = (AllowPrivateAccess = true));
	FVector stage2_Location;
};
