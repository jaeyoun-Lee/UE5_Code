// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"
#include "ABWeapon.h"
#include "ABCharacter.h"

// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	ItemBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Box"));
	effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	RootComponent = trigger;
	ItemBox->SetupAttachment(trigger);
	effect->SetupAttachment(trigger);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (tempMesh.Succeeded())
	{
		ItemBox->SetStaticMesh(tempMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> openEffect(TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if (openEffect.Succeeded())
	{
		effect->SetTemplate(openEffect.Object);
		effect->bAutoActivate = false;	// 파티클 시스템이 생성되자마자 자동으로 켜질지에 대한 변수
	}
	
	trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));	//  콜리전 박스 크기 설정 EXtent 값은 전체 박스 영역 크기의 절반 값을 의미한다.
	ItemBox->SetRelativeLocation(FVector(0, -3.5f, -30.0f));
	
	trigger->SetCollisionProfileName(TEXT("ItemBox"));
	ItemBox->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AABWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
}

void AABItemBox::PostInitializeComponents() 
{
	Super::PostInitializeComponents();

	trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnChracterOverlap);
}

// Called every frame
void AABItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABItemBox::OnChracterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);

	auto ABCharacter = Cast<AABCharacter>(OtherActor);	// 부딪힌 대상 액터에 정보를 가져온 매개변수
	ABCHECK(ABCharacter != nullptr);

	if (ABCharacter != nullptr && WeaponItemClass != nullptr)
	{
		if (ABCharacter->CanSetWeapon())
		{
			// ItemBox에 설정된 클래스 정보(WeaponItemClass)로부터 무기를 생성하고,
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			ABCharacter->SetWeapon(NewWeapon);	// 무기를 캐릭터에게 장착

			effect->Activate(true);	// overlap됐을 때, 파티클 재생
			ItemBox->SetHiddenInGame(true, true);	// 상자를 보이지 않도록(Destroy되기 전까지)
			SetActorEnableCollision(false);			// 상자의 충돌 설정도 없애버림
			effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
		}

		else
		{
			ABLOG(Warning, TEXT("%s can't equip weapon currently."), *ABCharacter->GetName());
		}
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();	// 액터 제거
}

