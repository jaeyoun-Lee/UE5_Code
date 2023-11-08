// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"		// 스켈레탈 메시 경로 저장되어있는 파일
#include "ABGameInstance.h"			// 스켈레탈 메시를 비동기로 적용시키기 위한 StreamableManager를 사용하기 위해
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"				// 지정한 캐릭터 및 캐릭터 이름을 저장하기 위해서
#include "ABPlayerState.h"


void UABCharacterSelectWidget::NextCharacter(bool bForward)
{
	// 왼쪽 버튼 눌렀는지 ? 오른쪽 버튼 눌렀는지 ?
	bForward ? CurrentIndex++ : CurrentIndex--;

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;	// 첫번째 스켈레탈 메시보다 작으면 맨 뒤 스켈레탈 메시 가져옴
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;			// 마지막 스켈레탈 메시보다 크면 맨 앞 스켈레탈 메시 가져옴

	auto CharacterSetting = GetDefault<UABCharacterSetting>();			// 메모리에 올라간 클래스 기본 객체는 GetDefault로 가져올 수 있음
	auto assetRef = CharacterSetting->CharacterAssets[CurrentIndex];	// 해당 인덱스에 위치한 스켈레탈 메시 경로를 가져옴

	auto GI = GetWorld()->GetGameInstance<UABGameInstance>();
	ABCHECK(GI != nullptr);
	ABCHECK(targetComponent.IsValid());

	// 게임 진행 중에도 비동기 방식으로 애셋을 로딩하도록 StreamableManager 사용
	// LoadSynchronous<T>()를 사용하여 참조된 애셋을 동기적으로 로드하고, 로드된 객체를 반환하거나 찾을 수 없으면 nullptr 반환
	USkeletalMesh* asset = GI->StreamableManager.LoadSynchronous<USkeletalMesh>(assetRef);

	if (asset != nullptr)
	{
		// 참조한 애셋이 있으면, 해당 애셋으로 스켈레탈 메시 설정
		targetComponent->SetSkeletalMesh(asset);
	}
}

// UI 시스템이 준비되면 호출하는 함수(초기화)
void UABCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;	// 초기 스켈레탈 메시 index = 0
	auto CharacterSetting = GetDefault<UABCharacterSetting>();	// 메모리에 올라간 클래스 기본 객체는 GetDefault로 가져올 수 있음
	MaxIndex = CharacterSetting->CharacterAssets.Num();			// 가져온 스켈레탈 메시의 개수

	
	// 현재 월드에 있는 ASkeletalMeshActor를 상속받은 액터의 목록 중 하나를 가져와서
	// 그 액터의 스켈레탈 메시를 targetComponent으론 설정
	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
	{
		targetComponent = It->GetSkeletalMeshComponent();
		break;
	}

	PrevButton = Cast<UButton>(GetWidgetFromName("btnPrev"));				// UI 위젯의 이전 버튼으로 지정
	ABCHECK(PrevButton != nullptr);

	NextButton = Cast<UButton>(GetWidgetFromName("btnNext"));				// UI 위젯의 다음 버튼으로 지정
	ABCHECK(NextButton != nullptr);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName("edtPlayerName"));	// 캐릭터 이름을 적을 텍스트 박스
	ABCHECK(TextBox != nullptr);

	ConfirmButton = Cast<UButton>(GetWidgetFromName("btnConfirm"));			// UI 위젯의 캐릭터 생성 버튼으로 지정
	ABCHECK(ConfirmButton != nullptr);

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);			// 이전 버튼 클릭 시,
	NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);			// 다음 버튼 클릭 시,
	ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);		// 캐릭터 생성 버튼 클릭 시,
}


void UABCharacterSelectWidget::OnPrevClicked()	// 이전 버튼이 눌리면 호출되는 함수
{
	NextCharacter(false);	// false 전달 시, 현재 스켈레탈 메시의 인덱스 - 1
}

void UABCharacterSelectWidget::OnNextClicked()	// 다음 버튼이 눌리면 호출되는 함수
{
	NextCharacter(true);	// true 전달 시, 현재 스켈레탈 메시의 인덱스 + 1
}

void UABCharacterSelectWidget::OnConfirmClicked()	// 캐릭터 생성 버튼이 눌리면 호출되는 함수
{
	FString name = TextBox->GetText().ToString();	// 캐릭터 이름 작성하는 UEditableTextBox에 쓰여있는 글자 가져오기
	if (name.Len() <= 0 || name.Len() > 10) return;	// 캐릭터 이름이 0자 이하, 10자 초과인 경우

	
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();	// 게임 데이터를 저장하는 세이브 파일 인스턴스 생성
	NewPlayerData->playerName = name;	// 캐릭터 초기 이름 설정
	NewPlayerData->level = 1;			// 캐릭터 초기 레벨 설정
	NewPlayerData->exp = 0;				// 캐릭터 초기 경험치 설정
	NewPlayerData->highScore = 0;		// 캐릭터 초기 최대 스코어 설정
	NewPlayerData->CharacterIndex = CurrentIndex;	// 캐릭터 초기 스켈레탈 메시 인덱스

	auto ABPlayerState = GetDefault<AABPlayerState>();	// 메모리에 올라간 클래스 기본 객체는 GetDefault로 가져올 수 있음

	// SaveSlotName의 저장 파일에 NewPlayerData 저장
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0))	// SaveSlotName 변수는 플레이어 스테이트로 선언된 변수로 "Player1"이다.
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("GamePlay"));	// GamePlay 레벨 열기
	}
	else
	{
		ABLOG(Error, TEXT("SaveGame Error !"));
	}
}