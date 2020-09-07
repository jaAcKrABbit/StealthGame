// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include <FPSGame\Public\FPSGameMode.h>
#include"Blueprint/AIBlueprintHelperLibrary.h"
#include "net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	OriginalRotation = GetActorRotation();
	if (bPatrol) MoveToNextPatrolPoint();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn) {
	if (SeenPawn == nullptr) return;
	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) GM->CompleteMission(SeenPawn, false);
	
	DrawDebugSphere(GetWorld(),SeenPawn->GetActorLocation(),32.0f,12,FColor::Yellow,false,10.0f);
	SetGuardState(EAIState::Alerted);
	if (GetController()) GetController()->StopMovement();
	
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInsitigator, const FVector& Location, float Volume) {
	if (NoiseInsitigator == nullptr) return;
	if (GuardState == EAIState::Alerted) return;

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();
	
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation,3.0f);
	
	SetGuardState(EAIState::Suspicious);
	if (GetController()) GetController()->StopMovement();

	DrawDebugSphere(GetWorld(), Location , 32.0f, 12, FColor::Green, false, 10.0f);
}

void AFPSAIGuard::ResetOrientation() {
	if (GuardState == EAIState::Alerted) return;
	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);
	if (bPatrol) MoveToNextPatrolPoint();
}

void AFPSAIGuard::OnRep_GuardState() {
	OnstateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState) {
	if (GuardState == NewState) return;
	
	GuardState = NewState;
    OnRep_GuardState();
}

void AFPSAIGuard::MoveToNextPatrolPoint() {
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint) CurrentPatrolPoint = FirstPatrolPoint;
	else CurrentPatrolPoint = SecondPatrolPoint;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), CurrentPatrolPoint->GetActorLocation());
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPatrolPoint) {
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();
		if (DistanceToGoal < 100) MoveToNextPatrolPoint();

	}

}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}


