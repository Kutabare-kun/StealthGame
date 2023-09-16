// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBlackHole.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	InnerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphere"));
	InnerSphere->SetSphereRadius(100.f);
	InnerSphere->SetupAttachment(MeshComp);

	InnerSphere->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::InnerOverlap);

	OuterSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphere"));
	OuterSphere->SetSphereRadius(2500.f);
	OuterSphere->SetupAttachment(MeshComp);
}


void AFPSBlackHole::InnerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherActor->Destroy();
	}
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> Components;
	OuterSphere->GetOverlappingComponents(Components);

	for (auto& Component : Components)
	{
		if (Component && Component->IsSimulatingPhysics())
		{
			const float SphereRadius = OuterSphere->GetScaledSphereRadius();
			const float ForceStrength = -2000.f;

			Component->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, RIF_Constant, true);

			// My Solution
			// FVector BlackHoleLocation = GetActorLocation();
			// FVector ComponentLocation = Component->GetComponentLocation();
			//
			// FVector MagnitudeTo = BlackHoleLocation - ComponentLocation;
			// float Strenght = 10.f;
			//
			// Component->AddForce(MagnitudeTo * Strenght, "NONE", true);
		}
	}
}

