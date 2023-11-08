// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GBEnemySetting.generated.h"

/**
 * 
 */
UCLASS(config=GBGame)
class ARENABATTLESETTING_API UGBEnemySetting : public UObject
{
	GENERATED_BODY()

public:
	UGBEnemySetting();

	// ������ config = GBGame���� �����Ͽ�, GBGame �������� .ini������ ã�´�.
	// �ű⼭ DefautlGBGame.ini�� ��� ��ΰ� [/Script/ArenaBattleSetting.GBEnemySetting] �̹Ƿ�, ���⿡ ini ������ �������ش�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;		// Grux ���̷�Ż �޽� ���


	// ������ config = GBGame���� �����Ͽ�, GBGame �������� .ini������ ã�´�.
	// �ű⼭ DefautlGBGame.ini�� ��� ��ΰ� [/Script/ArenaBattleSetting.GBEnemySetting] �̹Ƿ�, ���⿡ ini ������ �������ش�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> KhaimeraAssets;		// Khaimera ���̷�Ż �޽� ���


	// ������ config = GBGame���� �����Ͽ�, GBGame �������� .ini������ ã�´�.
	// �ű⼭ DefautlGBGame.ini�� ��� ��ΰ� [/Script/ArenaBattleSetting.GBEnemySetting] �̹Ƿ�, ���⿡ ini ������ �������ش�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> MorigeshAssets;		// Morigesh ���̷�Ż �޽� ���

	// ������ config = GBGame���� �����Ͽ�, GBGame �������� .ini������ ã�´�.
	// �ű⼭ DefautlGBGame.ini�� ��� ��ΰ� [/Script/ArenaBattleSetting.GBEnemySetting] �̹Ƿ�, ���⿡ ini ������ �������ش�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> RampageAssets;		// Morigesh ���̷�Ż �޽� ���

	// ������ config = GBGame���� �����Ͽ�, GBGame �������� .ini������ ã�´�.
	// �ű⼭ DefautlGBGame.ini�� ��� ��ΰ� [/Script/ArenaBattleSetting.GBEnemySetting] �̹Ƿ�, ���⿡ ini ������ �������ش�.
	UPROPERTY(config)
	TArray<FSoftObjectPath> SevarogAssets;		// Morigesh ���̷�Ż �޽� ���
};
