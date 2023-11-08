// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"
#include "ABCharacter.h"	// NPC를 생성하기 위해서
#include "ABItemBox.h"		// ItemBox를 생성하기 위해서
#include "ABPlayerController.h"
#include "ABGameMode.h"

// Sets default values
AABSection::AABSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 필드 스태틱 메시
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


	// 문 스태틱 메시
	FString doorPath = TEXT("/Script/Engine.StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");	// 문 스태틱 메시 경로
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempDoor(*doorPath);								// 문 스태틱 메시 할당
	if (!tempDoor.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"), *doorPath);						// 잘 가져왔는지 ?
	}

	static FName doorSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };	// 위 sectionMesh(필드)의 소켓 이름을 가져옴
	for (FName doorSocket : doorSockets)
	{
		// 문 스태틱 메시 추가
		ABCHECK(sectionMesh->DoesSocketExist(doorSocket));														// 소켓 이름이 존재하는지 검사
		UStaticMeshComponent* newDoor = CreateDefaultSubobject<UStaticMeshComponent>(*doorSocket.ToString());	// 소켓 이름과 똑같은 스태틱 메시 컴포넌트 생성
		newDoor->SetupAttachment(sectionMesh, doorSocket);														// 필드의 컴포넌트로 부착
		newDoor->SetStaticMesh(tempDoor.Object);																// 문 스태틱 메시 적용
		newDoor->SetRelativeLocation(FVector(0, -80.5f, 0));													// 위치 조절
		sectionDoors.Add(newDoor);															// 문이 여러 개이므로 TArray로 묶어서 관리하기 위해 TArray에 문 추가


		// 문에 대한 박스 콜리전 추가
		UBoxComponent* newCollision = CreateDefaultSubobject<UBoxComponent>(*doorSocket.ToString().Append("Trigger"));
		newCollision->SetupAttachment(sectionMesh, doorSocket);
		newCollision->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		newCollision->SetRelativeLocation(FVector(70.0f, 0, 250.0f));
		newCollision->SetCollisionProfileName(TEXT("ABTrigger"));		// ABCharacter만 overlap발생하는 프리셋으로 설정
		doorCollisions.Add(newCollision);								// 문에 대한 콜리전이 여러개로 TArray이 묶어서 관리하기 위해 TArray에 박스 콜리전 추가

		/// 문에 대한 박스 콜리전에 닿을 경우, 호출
		newCollision->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnDoorTriggerBeginOverlap);	
		newCollision->ComponentTags.Add(doorSocket);		// 색션을 생성할 방향을 태그로 추가
	}

	// 필드에 대한 박스 콜리전
	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGEER"));
	collision->SetupAttachment(sectionMesh);
	collision->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	collision->SetRelativeLocation(FVector(0, 0, 250.0f));
	collision->SetCollisionProfileName(TEXT("ABTrigger"));				// ABCharacter만 overlap발생하는 프리셋으로 설정
	
	/// 필드에 대한 박스 콜리전에 닿을 경우, 호출
	collision->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	bNoBattle = false;		// 시작할 때, 전투가 없이 완료 스테이트에서 시작할 수 있게 bNoBattle 속성을 사용

	// NPC, ItemBox 생성시간 초기화
	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 5.0f;
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작할 때, 전투가 없이 완료 스테이트에서 시작할 수 있게 bNoBattle 속성을 사용
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
	FName ComponentTag = OverlappedComponent->ComponentTags[0];		// 섹션을 생성할 방향을 저장한 태그
	FName SocketName = FName(*ComponentTag.ToString().Left(2));		// 섹션의 앞에 2글자 { +X, -X, +Y, -Y } 중 하나

	if (!sectionMesh->DoesSocketExist(SocketName))	return;
	FVector NewLocation = sectionMesh->GetSocketLocation(SocketName);		// 소켓 위치 가져오기

	TArray<FOverlapResult> OverlapResults;																// 부딪힌 액터 목록
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);									// 단순 충돌 모양만 고려
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);	// 찾고있는 객체 유형

	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,								// 탐색된 모든 객체를 담는 TArray
		NewLocation,								// 탐색할 모양의 위치
		FQuat::Identity,							// 충돌 형상에 적용할 회전으로, 충돌 검사에 회전 적용 X
		ObjectQueryParam,							// 모든 오브젝트를 찾음
		FCollisionShape::MakeSphere(775.0f),		// 충돌 검사를 확인할 영역으로 775 반경의 원으로 충돌 탐색
		CollisionQueryParam							// 단순 충돌 모양만 고려
	);

	if (!bResult)	// 아무것도 못찾으면(섹션이 없으면,) 섹션 생성
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else
		ABLOG(Warning, TEXT("New Section area is not Empty"));	// 이미 섹션이 있는 장소임(생성할 필요 X)
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
			OperateGates(true);	// 문을 열고 닫는 함수
			break;
		}
		
		case ESectionState::BATTLE :
		{
			collision->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* doorCollision : doorCollisions)
			{
				doorCollision->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(false);	// 문을 열고 닫는 함수

			// NPC 생성 타이머 생성 및 설정, 지정한 시간이되면 OnNPCSpawn() 함수 호출
			GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);

			// ItemBox 생성 타이머 생성 및 설정, 지정한 시간이 되면 람다식으로 작성한 함수 실행
			GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimeHandle, FTimerDelegate::CreateLambda([this]()-> void {
				FVector2D RandXY = FMath::RandPointInCircle(600.0f);	// 지정된 반경 내에서 임의의 점 반환
				GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);	// 지정한 위치(임의의 XY좌표 + Z좌표 30)에 ItemBox 생성
				
				}), ItemBoxSpawnTime, false);		// ~ 타이머 설정

			break;
		}

		case ESectionState::COMPLETE :
		{
			collision->SetCollisionProfileName(TEXT("NoCollision"));
			for (UBoxComponent* doorCollision : doorCollisions)
			{
				doorCollision->SetCollisionProfileName(TEXT("ABTrigger"));
			}
			OperateGates(true);		// 문을 열고 닫는 함수
			break;
		}
	}

	CurrentState = NewState;	// 매개변수로 받은 스테이트로 현재 스테이트 저장
}

// 문을 열고 닫는 함수
void AABSection::OperateGates(bool bOpen)
{
	for (UStaticMeshComponent* gate : sectionDoors)
	{
		// 문 4개 회전하여 열거나 닫기
		gate->SetRelativeRotation(bOpen ? FRotator(0, 90.0f, 0) : FRotator::ZeroRotator);
	}
}


// 액터와 컴포넌트 속성을 설정하면 작업 중인 레벨에서 미리 결과를 확인할 수 있게 해주는 함수
void AABSection::OnConstruction(const FTransform& transform)
{
	Super::OnConstruction(transform);

	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// NPC 생성함수
void AABSection::OnNPCSpawn()
{
	// NPC 생성 타이머 제거
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
	// 섹션의 중점에서 Z축 88을 올린 위치에 NPC 생성
	auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);

	if (KeyNPC != nullptr)
	{
		// 생성한 NPC가 죽으면 OnKeyNPCDestroyed() 함수 호출
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
	}
}

void AABSection::OnKeyNPCDestroyed(AActor* DestroyedActor)
{
	auto ABCharacter = Cast<AABCharacter>(DestroyedActor);	// 제거된 NPC 가져오기
	ABCHECK(ABCharacter != nullptr);

	// 마지막으로 공격한 캐릭터의 컨트롤러 가져오기(= 플레이어)
	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);	
	ABCHECK(ABPlayerController != nullptr);

	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(ABGameMode != nullptr);
	ABGameMode->AddScore(ABPlayerController);	// 게임모드의 AddScore 함수 호출하여 게임 스코어 증가 

	SetState(ESectionState::COMPLETE);	// NPC가 죽었으니 COMPLETE 상태로 변경
}