// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBaseComponent.h"

// Sets default values for this component's properties
UPlayerBaseComponent::UPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}


// Called when the game starts
void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// GetOwner()�Լ��� ������Ʈ �����ϰ��ִ� ������ �ν��Ͻ��� ������
	// ������ ���� ATPSPlayer Ÿ������ �ٽ� ĳ����
	me = Cast<ATPSPlayer>(GetOwner());
	
	// ĳ������ Ÿ���� CharacterMovementComponent ����
	moveComp = me->GetCharacterMovement();
	
}


// Called every frame
void UPlayerBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// BeginPlay() ���� ���� ȣ��Ǵ� ������ ����Ŭ �Լ�
void UPlayerBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();

	me = Cast<ATPSPlayer>(GetOwner());
	moveComp = me->GetCharacterMovement();

	// ��������Ʈ�� ó���Լ� ���
	me->onInputBindingDelegate.AddUObject(this, &UPlayerBaseComponent::SetupInputBinding);
}