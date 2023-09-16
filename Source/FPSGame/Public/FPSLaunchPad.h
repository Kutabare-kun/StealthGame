// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBoxComponent* OverlapComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* PlateComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* ArrowComp;

	UPROPERTY(EditDefaultsOnly, Category = "Launch|FX")
	UParticleSystem* LaunchEffect;
	
	UPROPERTY(EditAnywhere, Category = "Launch|Params")
	float LaunchPitchAngle;

	UPROPERTY(EditAnywhere, Category = "Launch|Params")
	float Strenght;

	UFUNCTION()
	void OverlapLaunch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
