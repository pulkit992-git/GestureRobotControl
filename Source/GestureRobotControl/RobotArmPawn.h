// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void HandleMoveXY(const FInputActionValue& Value);
	void HandleMoveZ(const FInputActionValue& Value);

	FVector CurrentInputDirection;
};
