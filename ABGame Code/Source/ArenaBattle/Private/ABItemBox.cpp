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
		effect->bAutoActivate = false;	// ��ƼŬ �ý����� �������ڸ��� �ڵ����� �������� ���� ����
	}
	
	trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));	//  �ݸ��� �ڽ� ũ�� ���� EXtent ���� ��ü �ڽ� ���� ũ���� ���� ���� �ǹ��Ѵ�.
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

	auto ABCharacter = Cast<AABCharacter>(OtherActor);	// �ε��� ��� ���Ϳ� ������ ������ �Ű�����
	ABCHECK(ABCharacter != nullptr);

	if (ABCharacter != nullptr && WeaponItemClass != nullptr)
	{
		if (ABCharacter->CanSetWeapon())
		{
			// ItemBox�� ������ Ŭ���� ����(WeaponItemClass)�κ��� ���⸦ �����ϰ�,
			auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			ABCharacter->SetWeapon(NewWeapon);	// ���⸦ ĳ���Ϳ��� ����

			effect->Activate(true);	// overlap���� ��, ��ƼŬ ���
			ItemBox->SetHiddenInGame(true, true);	// ���ڸ� ������ �ʵ���(Destroy�Ǳ� ������)
			SetActorEnableCollision(false);			// ������ �浹 ������ ���ֹ���
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
	Destroy();	// ���� ����
}

