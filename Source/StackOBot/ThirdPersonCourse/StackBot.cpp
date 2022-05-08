// Fill out your copyright notice in the Description page of Project Settings.


#include "../ThirdPersonCourse/StackBot.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/TimelineTemplate.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AStackBot::AStackBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 74.0f);

	//Follow Spring arm component
	FollowStackBotSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Follow Spring Arm"));
	FollowStackBotSpringArmComponent->SetupAttachment(RootComponent);
	FollowStackBotSpringArmComponent->TargetArmLength = 400.0f;
	FollowStackBotSpringArmComponent->bUsePawnControlRotation = true;
	FollowStackBotSpringArmComponent->bEnableCameraLag = true;
	FollowStackBotSpringArmComponent->CameraLagSpeed = 5.0f;
	
	//Follow Camera component
	FollowStackBotCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera Component"));
	FollowStackBotCameraComponent->SetupAttachment(FollowStackBotSpringArmComponent, USpringArmComponent::SocketName);
	FollowStackBotCameraComponent->bUsePawnControlRotation = false;
	//FollowStackBotCameraComponent->SetAutoActivate(false);

	//Shoulder spring arm component
	ShoulderStackBotSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Shoulder Spring Arm"));
	ShoulderStackBotSpringArmComponent->SetupAttachment(RootComponent);
	ShoulderStackBotSpringArmComponent->TargetArmLength = 80.0f;

	//Shoulder camera component
	ShoulderStackBotCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Shoulder Camera Component"));
	ShoulderStackBotCameraComponent->SetupAttachment(ShoulderStackBotSpringArmComponent, USpringArmComponent::SocketName);
	FVector ShoulderCameraOffset{0, 40.0f, 0};
	ShoulderStackBotCameraComponent->AddLocalOffset(ShoulderCameraOffset);
	ShoulderStackBotCameraComponent->SetAutoActivate(false);

	CameraSwitchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	InterpFunction.BindUFunction(this, FName("TimeLineFloatReturn"));
	TimeLineFinished.BindUFunction(this, FName("OnTimelineFinished"));

	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	this->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

// Called when the game starts or when spawned
void AStackBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStackBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStackBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AStackBot::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AStackBot::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Switch Cameras", IE_Pressed, this, &AStackBot::SwitchCameras);

}

void AStackBot::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AStackBot::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AStackBot::TimelineFloatReturn(float value)
{
	ShoulderStackBotCameraComponent->SetRelativeLocation(FMath::Lerp(RightShoulderLocation, LeftShoulderLocation, value));
}

void AStackBot::OnTimelineFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Timeline finished"));
}

void AStackBot::SwitchCameras()
{
	if (CameraFloatCurve)
	{
		//Add the float curve to the timeline and connect it to the interp functions delegates
		CameraSwitchTimeline->AddInterpFloat(CameraFloatCurve, InterpFunction, FName("Alpha"));
		//Add our timeline finished function
		CameraSwitchTimeline->SetTimelineFinishedFunc(TimeLineFinished);

		//Setting vectors
		RightShoulderLocation = {0.0f, 40.0f, 0.0f};
		LeftShoulderLocation = {0.0f, -40.0f, 0.0f};

		//Setting timeline settings before we start it
		CameraSwitchTimeline->SetLooping(false);
		CameraSwitchTimeline->SetIgnoreTimeDilation(true);

		//Start our timeline
		if (CameraSwitchTimeline->GetPlaybackPosition() == 0.0f)
		{
			CameraSwitchTimeline->Play();
		}
		else
		{
			CameraSwitchTimeline->Reverse();
		}
		
	}
}

