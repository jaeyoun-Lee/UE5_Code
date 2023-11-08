// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"
#include "ABCharacter.h"	// NPC�� �����ϱ� ���ؼ�
#include "ABItemBox.h"		// ItemBox�� �����ϱ� ���ؼ�
#include "ABPlayerController.h"
#include "ABGameMode.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �ʵ� ����ƽ �޽�
	sectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SectionMesh"));
	RootComponent = sectionMesh;

	FString assetPath = TEXT("/Script/Engine.StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'");
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(*assetPath);
	if (tempMesh.Succeeded())
	{
		sectionMesh->SetStaticMesh(tempMesh.Object);
	}
	else
		ABLOG(Error, TEXT("Failed to load staticmesh asset. : %s"), *assetPath);


	// �� ����ƽ �޽�
	FString doorPath = TEXT("/Script/Engine.StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");	// �� ����ƽ �޽� ���
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempDoor(*doorPath);								// �� ����ƽ �޽� �Ҵ�
	if (!tempDoor.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *doorPath);						// �� �����Դ��� ?
	}

	static FName doorSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };	// �� sectionMesh(�ʵ�)�� ���� �̸��� ������
	for (FName doorSocket : doorSockets)
	{
		// �� ����ƽ �޽� �߰�
		ABCHECK(sectionMesh->DoesSocketExist(doorSocket));														// ���� �̸��� �����ϴ��� �˻�
		UStaticMeshComponent* newDoor = CreateDefaultSubobject<UStaticMeshComponent>(*doorSocket.ToString());	// ���� �̸��� �Ȱ��� ����ƽ �޽� ������Ʈ ����
		newDoor->SetupAttachment(sectionMesh, doorSocket);														// �ʵ��� ������Ʈ�� ����
		newDoor->SetStaticMesh(tempDoor.Object);																// �� ����ƽ �޽� ����
		newDoor->SetRelativeLocation(FVector(0, -80.5f, 0));													// ��ġ ����
		sectionDoors.Add(newDoor);															// ���� ���� ���̹Ƿ� TArray�� ��� �����ϱ� ���� TArray�� �� �߰�


		// ���� ���� �ڽ� �ݸ��� �߰�
		UBoxComponent* newCollision = CreateDefaultSubobject<UBoxComponent>(*doorSocket.ToString().Append("Trigger"));
		newCollision->SetupAttachment(sectionMesh, doorSocket);
		newCollision->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		newCollision->SetRelativeLocation(FVector(70.0f, 0, 250.0f));
		newCollision->SetCollisionProfileName(TEXT("ABTrigger"));		// ABCharacter�� overlap�߻��ϴ� ���������� ����
		doorCollisions.Add(newCollision);								// ���� ���� �ݸ����� �������� TArray�� ��� �����ϱ� ���� TArray�� �ڽ� �ݸ��� �߰�

		/// ���� ���� �ڽ� �ݸ����� ���� ���, ȣ��
		newCollision->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnDoorTriggerBeginOverlap);	
		newCollision->ComponentTags.Add(doorSocket);		// ������ ������ ������ �±׷� �߰�
	}

	// �ʵ忡 ���� �ڽ� �ݸ���
	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGEER"));
	collision->SetupAttachment(sectionMesh);
	collision->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	collision->SetRelativeLocation(FVector(0, 0, 250.0f));
	collision->SetCollisionProfileName(TEXT("ABTrigger"));				// ABCharacter�� overlap�߻��ϴ� ���������� ����
	
	/// �ʵ忡 ���� �ڽ� �ݸ����� ���� ���, ȣ��
	collision->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	bNoBattle = false;		// ������ ��, ������ ���� �Ϸ� ������Ʈ���� ������ �� �ְ� bNoBattle �Ӽ��� ���

	// NPC, ItemBox �����ð� �ʱ�ȭ
	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 5.0f;
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	
	// ������ ��, ������ ���� �Ϸ� ������Ʈ���� ������ �� �ְ� bNoBattle �Ӽ��� ���
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
										int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState == ESectionState::READY)
		SetState(ESectionState::BATTLE);
}

void AABSection::OnDoorTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];		// ������ ������ ������ ������ �±�
	FName SocketName = FName(*ComponentTag.ToString().Left(2));		// ������ �տ� 2���� { +X, -X, +Y, -Y } �� �ϳ�

	if (!sectionMesh->DoesSocketExist(SocketName))	return;
	FVector NewLocation = sectionMesh->GetSocketLocation(SocketName);		// ���� ��ġ ��������

	TArray<FOverlapResult> OverlapResults;																// �ε��� ���� ���
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);									// �ܼ� �浹 ��縸 ���
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);	// ã���ִ� ��ü ����

	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,								// Ž���� ��� ��ü�� ��� TArray
		NewLocation,								// Ž���� ����� ��ġ
		FQuat::Identity,							// �浹 ���� ������ ȸ������, �浹 �˻翡 ȸ�� ���� X
		ObjectQueryParam,							// ��� ������Ʈ�� ã��
		FCollisionShape::MakeSphere(775.0f),		// �浹 �˻縦 Ȯ���� �������� 775 �ݰ��� ������ �浹 Ž��
		CollisionQueryParam							// �ܼ� �浹 ��縸 ���
	);

	if (!bResult)	// �ƹ��͵� ��ã����(������ ������,) ���� ����
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else
		ABLOG(Warning, TEXT("New Section area is not Empty"));	// �̹� ������ �ִ� �����(������ �ʿ� X)
}


void AABSection::SetState(ESectionState NewState)
{
	switch (NewState)
	{
		case ESectionState::READY: 
		{
			collision->SetCollisionProfileName(TEXT("ABTrigger"));
			for (UBoxComponent* doorCollision : doorCollisions)
			{
				doorCollision->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(true);	// ���� ���� �ݴ� �Լ�
			break;
		}
		
		case ESectionState::BATTLE :
		{
			collision->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* doorCollision : doorCollisions)
			{
				doorCollision->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(false);	// ���� ���� �ݴ� �Լ�

			// NPC ���� Ÿ�̸� ���� �� ����, ������ �ð��̵Ǹ� OnNPCSpawn() �Լ� ȣ��
			GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);

			// ItemBox ���� Ÿ�̸� ���� �� ����, ������ �ð��� �Ǹ� ���ٽ����� �ۼ��� �Լ� ����
			GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimeHandle, FTimerDelegate::CreateLambda([this]()-> void {
				FVector2D RandXY = FMath::RandPointInCircle(600.0f);	// ������ �ݰ� ������ ������ �� ��ȯ
				GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);	// ������ ��ġ(������ XY��ǥ + Z��ǥ 30)�� ItemBox ����
				
				}), ItemBoxSpawnTime, false);		// ~ Ÿ�̸� ����

			break;
		}

		case ESectionState::COMPLETE :
		{
			collision->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* doorCollision : doorCollisions)
			{
				doorCollision->SetCollisionProfileName(TEXT("ABTrigger"));
			}
			OperateGates(true);		// ���� ���� �ݴ� �Լ�
			break;
		}
	}

	CurrentState = NewState;	// �Ű������� ���� ������Ʈ�� ���� ������Ʈ ����
}

// ���� ���� �ݴ� �Լ�
void AABSection::OperateGates(bool bOpen)
{
	for (UStaticMeshComponent* gate : sectionDoors)
	{
		// �� 4�� ȸ���Ͽ� ���ų� �ݱ�
		gate->SetRelativeRotation(bOpen ? FRotator(0, 90.0f, 0) : FRotator::ZeroRotator);
	}
}


// ���Ϳ� ������Ʈ �Ӽ��� �����ϸ� �۾� ���� �������� �̸� ����� Ȯ���� �� �ְ� ���ִ� �Լ�
void AABSection::OnConstruction(const FTransform& transform)
{
	Super::OnConstruction(transform);

	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// NPC �����Լ�
void AABSection::OnNPCSpawn()
{
	// NPC ���� Ÿ�̸� ����
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
	// ������ �������� Z�� 88�� �ø� ��ġ�� NPC ����
	auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);

	if (KeyNPC != nullptr)
	{
		// ������ NPC�� ������ OnKeyNPCDestroyed() �Լ� ȣ��
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
	}
}

void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
{
	auto ABCharacter = Cast<AABCharacter>(DestroyedActor);	// ���ŵ� NPC ��������
	ABCHECK(ABCharacter != nullptr);

	// ���������� ������ ĳ������ ��Ʈ�ѷ� ��������(= �÷��̾�)
	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);	
	ABCHECK(ABPlayerController != nullptr);

	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(ABGameMode != nullptr);
	ABGameMode->AddScore(ABPlayerController);	// ���Ӹ���� AddScore �Լ� ȣ���Ͽ� ���� ���ھ� ���� 

	SetState(ESectionState::COMPLETE);	// NPC�� �׾����� COMPLETE ���·� ����
}