// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPawn.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character"));
	moveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MoveComponent"));
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	RootComponent = collision;
	meshComp->SetupAttachment(collision);
	springArm->SetupAttachment(collision);
	camComp->SetupAttachment(springArm);

	collision->SetCapsuleHalfHeight(88.0f);
	collision->SetCapsuleRadius(34.0f);
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -88.0f), FRotator(0, -90, 0));
	springArm->TargetArmLength = 400.0f;
	springArm->SetRelativeRotation(FRotator(-15, 0, 0));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (tempMesh.Succeeded()) {
		meshComp->SetSkeletalMesh(tempMesh.Object);
	}
	
	meshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> warrior_Anim(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Warrior.ABP_Warrior_C'"));

	if (warrior_Anim.Succeeded()) {
		meshComp->SetAnimInstanceClass(warrior_Anim.Class);
	}
}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//meshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/Animation/WarriorRun.WarriorRun'"));

	//if (AnimAsset != nullptr) {
	//	meshComp->PlayAnimation(AnimAsset, true);
	//}
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPawn::LeftRight);
}

void AABPawn::PostInitializeComponents() {
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPawn::PossessedBy(AController* NewController)
{
	ABLOG_S(Warning);
	Super::PossessedBy(NewController);
}


void AABPawn::UpDown(float NewAxisValue) 
{
	//ABLOG(Warning, TEXT("%f"), NewAxisValue);
	AddMovementInput(GetActorForwardVector(), NewAxisValue);
}

void AABPawn::LeftRight(float NewAxisValue) 
{
	//ABLOG(Warning, TEXT("%f"), NewAxisValue);
	AddMovementInput(GetActorRightVector(), NewAxisValue);
}