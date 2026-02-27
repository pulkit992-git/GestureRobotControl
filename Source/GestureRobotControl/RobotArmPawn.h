// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "RobotArmPawn.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class GESTUREROBOTCONTROL_API ARobotArmPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARobotArmPawn();

	UPROPERTY (BlueprintReadWrite, Category = "Robot Control")
	FVector IKTargetLocation;

	UPROPERTY (EditAnywhere, Category = "Robot Control")
	float MoveSpeed = 100.0f;

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

	UPROPERTY(VisibleAnywhere, Category = "Physics")
	class UPhysicsConstraintComponent* LeftFingerConstraint;

	UPROPERTY(VisibleAnywhere, Category = "Physics")
	class UPhysicsConstraintComponent* RightFingerConstraint;

	UPROPERTY(EditAnywhere, Category = "Physics")
	float GripStrength = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Physics")
	bool bIsClawClosed;

	void UpdateClawState(bool bShouldClose);

private:
	void HandleMoveXY(const FInputActionValue& Value);
	void HandleMoveZ(const FInputActionValue& Value);

	FVector CurrentInputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* LeftFingerConstraint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* RightFingerConstraint;
};
