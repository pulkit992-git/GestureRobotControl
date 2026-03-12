// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RobotArmPawn.generated.h"


class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UENUM (BlueprintType)
enum class ERobotState : uint8
{
	Idle,
	MovingToPick,
	MovingToPlace
};

UCLASS()
class GESTUREROBOTCONTROL_API ARobotArmPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARobotArmPawn();

	UPROPERTY (BlueprintReadWrite, Category = "Robot Control")
	FVector IKTargetLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Robot Control")
	FRotator IKTargetRotation;

	FVector TargetLocation;
	FRotator TargetRotation;
	
	FVector CurrentLocation;
	FRotator CurrentRotation;

	UPROPERTY (EditAnywhere, Category = "Robot Control")
	float MoveSpeed = .1f;

	// The Spring Arm (controls camera distance/rotation)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	// The actual Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveXYAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveZAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ClawAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseClick;

	UPROPERTY()
	FVector RobotBaseLocation;

	UPROPERTY(EditAnywhere)
	float MinimumSafeRadius = 30.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void UpdateKinematics(FVector2D MoveXY, float MoveZ, bool Grab);

	UFUNCTION()
	void HandleMouseClick();

	UFUNCTION()
	void UpdateIKTarget(FVector ClickedPoint, AActor* HitActor);

	UPROPERTY(VisibleAnywhere, Category = "Physics")
	class UPhysicsConstraintComponent* LeftFingerConstraint;

	UPROPERTY(VisibleAnywhere, Category = "Physics")
	class UPhysicsConstraintComponent* RightFingerConstraint;

	UPROPERTY(EditAnywhere, Category = "Physics")
	float GripStrength = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Physics")
	bool bIsClawClosed = false;

	void OnToggleClaw();

	void UpdateClawState(bool bShouldClose);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot")
	USkeletalMeshComponent* MeshComponent;

private:
	void HandleMoveXY(const FInputActionValue& Value);
	void HandleMoveZ(const FInputActionValue& Value);

	FVector CurrentInputDirection;

	ERobotState CurrentState;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	//class UPhysicsConstraintComponent* LeftFingerConstraint;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	//class UPhysicsConstraintComponent* RightFingerConstraint;
};
