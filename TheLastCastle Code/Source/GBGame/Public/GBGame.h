// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(GBGame, Log, All);

#define GBLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define GBLOG_S(Verbosity) UE_LOG(GBGame, Verbosity, TEXT("%s"), *GBLOG_CALLINFO)

#define GBLOG(Verbosity, Format, ...) UE_LOG(GBGame, Verbosity, TEXT("%s%s"), *GBLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// ABCHECK ��ũ�� ���� ������ ���� �ƴ� ��� �αװ� �Ӱ� ���
#define GBCHECK(Expr, ...) {if(!(Expr)) {GBLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }
