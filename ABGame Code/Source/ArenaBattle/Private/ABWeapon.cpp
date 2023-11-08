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

	AttackRange = 150.0f;		// 무기를 들었을 때, 기본 공격 범위
	AttackDamageMin = -2.5f;	// 무기 공격력 최솟값
	AttackDamageMax = 10.0f;	// 무기 공격력 최댓값
	AttackModifierMin = 0.85f;	// 효과치 최솟값
	AttackModifierMax = 1.25f;	// 효과치 최댓값
}

float AABWeapon::GetAttackDamage() const
{
	return AttackDamage;	// 무기 공격력 반환
}

float AABWeapon::GetAttackModifier() const
{
	return AttackModifier;	// 효과치 반환
}


// Called when the game starts or when spawned
void AABWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	AttackDamage = FMath::RandRange(AttackDamageMin, AttackDamageMax);			// 무기 최소 공격력 ~ 최대 공격력 사이에서 랜덤 값을 AttackDamage에 저장
	AttackModifier = FMath::RandRange(AttackModifierMin, AttackModifierMax);	// 최소 효과치 ~ 최대 효과치 사이에서 랜덤 값을 AttackModifier에 저장

	ABLOG(Warning, TEXT("Weapon Damamge : %f, Modifier : %f"), AttackDamage, AttackModifier);
}

float AABWeapon::GetAttackRange() const
{
	return AttackRange;	// 무기 공격 범위 반환
}

// Called every frame
void AABWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

