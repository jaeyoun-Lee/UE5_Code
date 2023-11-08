// Fill out your copyright notice in the Description page of Project Settings.


#include "ABWeapon.h"

// Sets default values
AABWeapon::AABWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapons"));
	RootComponent = weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempWeapon(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight'"));

	if (tempWeapon.Succeeded())
	{
		weapon->SetSkeletalMesh(tempWeapon.Object);
	}

	weapon->SetCollisionProfileName(TEXT("NoCollision"));

	AttackRange = 150.0f;		// ���⸦ ����� ��, �⺻ ���� ����
	AttackDamageMin = -2.5f;	// ���� ���ݷ� �ּڰ�
	AttackDamageMax = 10.0f;	// ���� ���ݷ� �ִ�
	AttackModifierMin = 0.85f;	// ȿ��ġ �ּڰ�
	AttackModifierMax = 1.25f;	// ȿ��ġ �ִ�
}

float AABWeapon::GetAttackDamage() const
{
	return AttackDamage;	// ���� ���ݷ� ��ȯ
}

float AABWeapon::GetAttackModifier() const
{
	return AttackModifier;	// ȿ��ġ ��ȯ
}


// Called when the game starts or when spawned
void AABWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	AttackDamage = FMath::RandRange(AttackDamageMin, AttackDamageMax);			// ���� �ּ� ���ݷ� ~ �ִ� ���ݷ� ���̿��� ���� ���� AttackDamage�� ����
	AttackModifier = FMath::RandRange(AttackModifierMin, AttackModifierMax);	// �ּ� ȿ��ġ ~ �ִ� ȿ��ġ ���̿��� ���� ���� AttackModifier�� ����

	ABLOG(Warning, TEXT("Weapon Damamge : %f, Modifier : %f"), AttackDamage, AttackModifier);
}

float AABWeapon::GetAttackRange() const
{
	return AttackRange;	// ���� ���� ���� ��ȯ
}

// Called every frame
void AABWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

