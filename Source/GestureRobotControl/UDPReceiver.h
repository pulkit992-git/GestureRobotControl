// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Networking.h"
#include "Common/UdpSocketBuilder.h"
#include "RobotArmPawn.h"

#include "UDPReceiver.generated.h"

UCLASS()
class GESTUREROBOTCONTROL_API AUDPReceiver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUDPReceiver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ReceiveData();

	void ParseJsonData(const FString ReceivedText);

private:
	FSocket* ListenSocket;
	FTimerHandle ReceiverTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY (VisibleAnywhere)
	FVector2D MoveXY;

	UPROPERTY (VisibleAnywhere)
	bool Grab = false;

	UPROPERTY(VisibleAnywhere, Category = "Robot Control")
	class ARobotArmPawn* RoboArm;
};
