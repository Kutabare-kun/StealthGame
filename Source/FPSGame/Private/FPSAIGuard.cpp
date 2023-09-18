// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;

	CurrentPatrol = -1;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}


void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChange(GuardState);
}


void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}
	
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alerted);
}


void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}
	
	DrawDebugSphere(GetWorld(), Location, 28.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();
	
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);
	
	SetGuardState(EAIState::Suspicious);

	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}
}


void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}
	
	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}


void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;

	OnRep_GuardState();
}


void AFPSAIGuard::MoveToNextPatrolPoint()
{
	CurrentPatrol = (PatrolLocation.Num() - 1) == CurrentPatrol ? 0 : CurrentPatrol + 1;
	
	AActor* MoveToLocation = PatrolLocation[CurrentPatrol];

	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), MoveToLocation);
}


void AFPSAIGuard::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bPatrol && CurrentPatrol != -1)
	{
		const FVector Delta = GetActorLocation() - PatrolLocation[CurrentPatrol]->GetActorLocation();
		const float DistanceToGoal = Delta.Size();
		
		if (DistanceToGoal < 50.0f)
		{
			MoveToNextPatrolPoint();
		}
	}
}


void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}