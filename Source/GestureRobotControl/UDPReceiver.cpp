// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPReceiver.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "EngineUtils.h"

// Sets default values
AUDPReceiver::AUDPReceiver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUDPReceiver::BeginPlay()
{
	Super::BeginPlay();

	FIPv4Address Addr;
	FIPv4Address::Parse(TEXT("127.0.0.1"), Addr);
	FIPv4Endpoint Endpoint(Addr, 12345);
	
	// create socket
	ListenSocket = FUdpSocketBuilder(TEXT ("PythonUDP"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(2 * 1024 * 1024);

	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Error, TEXT ("UDP Socket creating failed"));
		return;
	}

	// check for data every 0.01 s
	GetWorld()->GetTimerManager().SetTimer(
		ReceiverTimer,
		this,
		&AUDPReceiver::ReceiveData,
		0.01f,
		true);

	UE_LOG(LogTemp, Warning, TEXT("UDP Receiver Started"));
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

void AUDPReceiver::ReceiveData()
{
	//UE_LOG(LogTemp, Display, TEXT("Checking in Receiver"));

	if (!ListenSocket) return;

	uint32 Size;
	while (ListenSocket->HasPendingData(Size))
	{
		TArray<uint8> Data;
		Data.SetNumUninitialized(FMath::Min(Size, 65507u));

		int32 BytesRead = 0;
		ListenSocket->Recv(Data.GetData(), Data.Num(), BytesRead);

		FString ReceivedText = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Data.GetData())));

		//UE_LOG(LogTemp, Warning, TEXT ("Received : %s"), *ReceivedText);

		ParseJsonData(ReceivedText);
	}
}

void AUDPReceiver::ParseJsonData(const FString ReceivedText)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader =
		TJsonReaderFactory<>::Create(ReceivedText);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		MoveXY.X = JsonObject->GetNumberField(TEXT("move_x"));
		MoveXY.Y = JsonObject->GetNumberField(TEXT("move_y"));
		
		Grab = JsonObject->GetIntegerField(TEXT("grab")) == 1;

		//ApplyInput(MoveX, MoveY, AimX, AimY, bShoot, bJump);
		UE_LOG(LogTemp, Display, TEXT ("X : %f, Y : %f, Grab : %d"), MoveXY.X, MoveXY.Y, Grab);

		if (RoboArm)
		{
			RoboArm->UpdateKinematics(MoveXY, 0.0f, Grab);
		}
		
	}
}

// Called every frame
void AUDPReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

