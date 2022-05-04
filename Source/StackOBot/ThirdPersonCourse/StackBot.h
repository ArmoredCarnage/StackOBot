// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "StackBot.generated.h"

UCLASS()
class STACKOBOT_API AStackBot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStackBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* FollowStackBotSpringArmComponent;
	
	UPROPERTY(EditAnywhere)
	class UCameraComponent* FollowStackBotCameraComponent;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* ShoulderStackBotSpringArmComponent;
	
	UPROPERTY(EditAnywhere)
	class UCameraComponent* ShoulderStackBotCameraComponent;
	
	class UTimelineComponent* CameraSwitchTimeline;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* CameraFloatCurve;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FVector EndLocation;

	//Declare our delegate function to be binded with TimelineFloatReturn(float value)
	FOnTimelineFloat InterpFunction{};

	//Declare our delegate function to be binded with OnTimelineFinished()
	FOnTimelineEvent TimeLineFinished{};

	UFUNCTION()
	void TimelineFloatReturn(float value);

	UFUNCTION()
	void OnTimelineFinished();

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION()
	void SwitchCameras();
};

