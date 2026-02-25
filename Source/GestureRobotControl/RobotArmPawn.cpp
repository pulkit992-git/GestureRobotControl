// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotArmPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "UDPReceiver.h"

// Sets default values
ARobotArmPawn::ARobotArmPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. Create a Root Component if you haven't (Scene Component is standard)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	// 2. Setup the Spring Arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // Attach to your root
	CameraBoom->TargetArmLength = 400.0f;       // Distance from the robot
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f)); // Angled view

	// 3. Setup the Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach to end of boom
}

// Called when the game starts or when spawned
void ARobotArmPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	RobotBaseLocation = GetActorLocation();

	AUDPReceiver* UDPReceiver;
	
	for (TActorIterator<AUDPReceiver> It(GetWorld()); It; ++It)
	{
		UDPReceiver = *It;
		if (UDPReceiver)
		{
			UDPReceiver->RoboArm = this;
			UE_LOG(LogTemp, Error, TEXT ("Assigned robo arm"));
			break;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't find"));
		}
	}
	
}

void ARobotArmPawn::UpdateKinematics(FVector2D MoveXY, float MoveZ, bool Grab)
{
	IKTargetLocation.X += MoveXY.X;
	IKTargetLocation.Y += MoveXY.Y;
}

// Called every frame
void ARobotArmPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrimaryActorTick.bCanEverTick = true;

	// set a default starting point
	//IKTargetLocation = FVector(50.0f, 0.0f, 50.0f);

	float CurrentDistance = FVector::Dist(IKTargetLocation, RobotBaseLocation);

	if (CurrentDistance < MinimumSafeRadius)
	{
		// 3. Push it to the closest safe point on the sphere's edge
		FVector DirectionFromBase = IKTargetLocation - RobotBaseLocation;
		DirectionFromBase.Normalize();
		IKTargetLocation = RobotBaseLocation + (DirectionFromBase * MinimumSafeRadius);
	}
}

// Called to bind functionality to input
void ARobotArmPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveXYAction, ETriggerEvent::Triggered, this, &ARobotArmPawn::HandleMoveXY);

		EnhancedInputComponent->BindAction(MoveZAction, ETriggerEvent::Triggered, this, &ARobotArmPawn::HandleMoveZ);
	}
}

void ARobotArmPawn::HandleMoveXY(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	IKTargetLocation.X += MovementVector.X;
	IKTargetLocation.Y += MovementVector.Y;

	UE_LOG(LogTemp, Display, TEXT ("%f %f"), MovementVector.X, MovementVector.Y);
}

void ARobotArmPawn::HandleMoveZ(const FInputActionValue& Value)
{
	float ZMovement = Value.Get<float>();
	IKTargetLocation.Z += ZMovement;

	UE_LOG(LogTemp, Display, TEXT("%f"), ZMovement);
}

