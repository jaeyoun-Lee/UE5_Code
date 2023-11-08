// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterSelectWidget.h"
#include "ABCharacterSetting.h"		// ���̷�Ż �޽� ��� ����Ǿ��ִ� ����
#include "ABGameInstance.h"			// ���̷�Ż �޽ø� �񵿱�� �����Ű�� ���� StreamableManager�� ����ϱ� ����
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "ABSaveGame.h"				// ������ ĳ���� �� ĳ���� �̸��� �����ϱ� ���ؼ�
#include "ABPlayerState.h"


void UABCharacterSelectWidget::NextCharacter(bool bForward)
{
	// ���� ��ư �������� ? ������ ��ư �������� ?
	bForward ? CurrentIndex++ : CurrentIndex--;

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;	// ù��° ���̷�Ż �޽ú��� ������ �� �� ���̷�Ż �޽� ������
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;			// ������ ���̷�Ż �޽ú��� ũ�� �� �� ���̷�Ż �޽� ������

	auto CharacterSetting = GetDefault<UABCharacterSetting>();			// �޸𸮿� �ö� Ŭ���� �⺻ ��ü�� GetDefault�� ������ �� ����
	auto assetRef = CharacterSetting->CharacterAssets[CurrentIndex];	// �ش� �ε����� ��ġ�� ���̷�Ż �޽� ��θ� ������

	auto GI = GetWorld()->GetGameInstance<UABGameInstance>();
	ABCHECK(GI != nullptr);
	ABCHECK(targetComponent.IsValid());

	// ���� ���� �߿��� �񵿱� ������� �ּ��� �ε��ϵ��� StreamableManager ���
	// LoadSynchronous<T>()�� ����Ͽ� ������ �ּ��� ���������� �ε��ϰ�, �ε�� ��ü�� ��ȯ�ϰų� ã�� �� ������ nullptr ��ȯ
	USkeletalMesh* asset = GI->StreamableManager.LoadSynchronous<USkeletalMesh>(assetRef);

	if (asset != nullptr)
	{
		// ������ �ּ��� ������, �ش� �ּ����� ���̷�Ż �޽� ����
		targetComponent->SetSkeletalMesh(asset);
	}
}

// UI �ý����� �غ�Ǹ� ȣ���ϴ� �Լ�(�ʱ�ȭ)
void UABCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;	// �ʱ� ���̷�Ż �޽� index = 0
	auto CharacterSetting = GetDefault<UABCharacterSetting>();	// �޸𸮿� �ö� Ŭ���� �⺻ ��ü�� GetDefault�� ������ �� ����
	MaxIndex = CharacterSetting->CharacterAssets.Num();			// ������ ���̷�Ż �޽��� ����

	
	// ���� ���忡 �ִ� ASkeletalMeshActor�� ��ӹ��� ������ ��� �� �ϳ��� �����ͼ�
	// �� ������ ���̷�Ż �޽ø� targetComponent���� ����
	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
	{
		targetComponent = It->GetSkeletalMeshComponent();
		break;
	}

	PrevButton = Cast<UButton>(GetWidgetFromName("btnPrev"));				// UI ������ ���� ��ư���� ����
	ABCHECK(PrevButton != nullptr);

	NextButton = Cast<UButton>(GetWidgetFromName("btnNext"));				// UI ������ ���� ��ư���� ����
	ABCHECK(NextButton != nullptr);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName("edtPlayerName"));	// ĳ���� �̸��� ���� �ؽ�Ʈ �ڽ�
	ABCHECK(TextBox != nullptr);

	ConfirmButton = Cast<UButton>(GetWidgetFromName("btnConfirm"));			// UI ������ ĳ���� ���� ��ư���� ����
	ABCHECK(ConfirmButton != nullptr);

	PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);			// ���� ��ư Ŭ�� ��,
	NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);			// ���� ��ư Ŭ�� ��,
	ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);		// ĳ���� ���� ��ư Ŭ�� ��,
}


void UABCharacterSelectWidget::OnPrevClicked()	// ���� ��ư�� ������ ȣ��Ǵ� �Լ�
{
	NextCharacter(false);	// false ���� ��, ���� ���̷�Ż �޽��� �ε��� - 1
}

void UABCharacterSelectWidget::OnNextClicked()	// ���� ��ư�� ������ ȣ��Ǵ� �Լ�
{
	NextCharacter(true);	// true ���� ��, ���� ���̷�Ż �޽��� �ε��� + 1
}

void UABCharacterSelectWidget::OnConfirmClicked()	// ĳ���� ���� ��ư�� ������ ȣ��Ǵ� �Լ�
{
	FString name = TextBox->GetText().ToString();	// ĳ���� �̸� �ۼ��ϴ� UEditableTextBox�� �����ִ� ���� ��������
	if (name.Len() <= 0 || name.Len() > 10) return;	// ĳ���� �̸��� 0�� ����, 10�� �ʰ��� ���

	
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();	// ���� �����͸� �����ϴ� ���̺� ���� �ν��Ͻ� ����
	NewPlayerData->playerName = name;	// ĳ���� �ʱ� �̸� ����
	NewPlayerData->level = 1;			// ĳ���� �ʱ� ���� ����
	NewPlayerData->exp = 0;				// ĳ���� �ʱ� ����ġ ����
	NewPlayerData->highScore = 0;		// ĳ���� �ʱ� �ִ� ���ھ� ����
	NewPlayerData->CharacterIndex = CurrentIndex;	// ĳ���� �ʱ� ���̷�Ż �޽� �ε���

	auto ABPlayerState = GetDefault<AABPlayerState>();	// �޸𸮿� �ö� Ŭ���� �⺻ ��ü�� GetDefault�� ������ �� ����

	// SaveSlotName�� ���� ���Ͽ� NewPlayerData ����
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0))	// SaveSlotName ������ �÷��̾� ������Ʈ�� ����� ������ "Player1"�̴�.
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("GamePlay"));	// GamePlay ���� ����
	}
	else
	{
		ABLOG(Error, TEXT("SaveGame Error !"));
	}
}