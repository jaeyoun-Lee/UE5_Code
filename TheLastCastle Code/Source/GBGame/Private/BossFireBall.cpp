// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFireBall.h"
#include "GBGameState.h"
#include "GBPlayerController.h"					// Include the appropriate player controller class header
#include "GameFramework/PlayerController.h"		// Include the PlayerController header
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"


// Sets default values
ABossFireBall::ABossFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempPlane(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (tempPlane.Succeeded())
	{
		plane->SetStaticMesh(tempPlane.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> planeMat(TEXT("/Script/Engine.Material'/Game/BP/Bullet/M_FireBall.M_FireBall'"));
	if (planeMat.Succeeded())
	{
		plane->SetMaterial(0, planeMat.Object);
	}


	RootComponent = collision;				// 루트 컴포넌트로 등록
	plane->SetupAttachment(collision);
	pointLight->SetupAttachment(collision);

	collision->SetSphereRadius(24);
	pointLight->Intensity = 3000.0f;
	pointLight->SetLightColor(FColor::FromHex("#FF6000FF"));
	pointLight->AttenuationRadius = 300.0f;
	
	repeatInterval = 0.01f;	// 타이머 반복하는 사이에 시간 텀 3초
}


void ABossFireBall::FaceFireBall()
{
	AGBPlayerController* GBPlayerController = Cast<AGBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	GBLOG(Warning, TEXT("Called FaceFireBall Function"));

	if (GBPlayerController != nullptr)
	{
		GBLOG(Warning, TEXT("Find to GBPlayerController"));
		APlayerCameraManager* camManager = GBPlayerController->PlayerCameraManager;
		if (camManager != nullptr)
		{
			GBLOG(Warning, TEXT("Find to CameraManager"));
			FVector camVector = camManager->GetCameraLocation() - GetActorLocation().Normalize(0.0001);
			FRotator camRot = FRotationMatrix::MakeFromX(camVector).Rotator();
			FRotator planeRot;
			planeRot.Roll = camRot.Roll;
			planeRot.Pitch = camRot.Pitch + 90;
			planeRot.Yaw = camRot.Yaw;
			
			GBLOG(Warning, TEXT("camRotation = %f, %f, %f"), camRot.Roll, camRot.Pitch, camRot.Yaw);
			GBLOG(Warning, TEXT("PlaneRotation = %f, %f, %f"), planeRot.Roll, planeRot.Pitch , planeRot.Yaw);
			plane->SetWorldRotation(FRotator(planeRot));
		}
	}
}


// Called when the game starts or when spawned
void ABossFireBall::BeginPlay()
{
	Super::BeginPlay();
	
	// 타이머 핸들로, OnRepeatTimer() 함수 실행, 3초마다, 반복해서
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &ABossFireBall::FaceFireBall, repeatInterval, true);
}

// Called every frame
void ABossFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

