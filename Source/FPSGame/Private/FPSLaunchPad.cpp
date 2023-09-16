// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"

#include "FPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{	
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	// OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// OverlapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// OverlapComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	OverlapComp->SetBoxExtent(FVector(75.f, 75.f, 20.f));
	RootComponent = OverlapComp;

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunch);

	PlateComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateComp"));
	PlateComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlateComp->SetupAttachment(RootComponent);

	ArrowComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowComp"));
	ArrowComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArrowComp->SetupAttachment(RootComponent);

	Strenght = 1750.f;
	LaunchPitchAngle = 35.f;
}


void AFPSLaunchPad::OverlapLaunch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch = LaunchPitchAngle;

	FVector LaunchVelocity = LaunchDirection.Vector() * Strenght;
	
	AFPSCharacter* MyPawn = Cast<AFPSCharacter>(OtherActor);
	if (MyPawn)
	{
		MyPawn->LaunchCharacter(LaunchVelocity, true, true);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LaunchEffect, GetActorLocation());
	}
	
	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LaunchEffect, GetActorLocation());
	}
}


