// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UStaticMeshComponent;
class UBoxComponent;


UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* OverlapComp;
	UFUNCTION()
	void OverlapLaunchPad(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditInstanceOnly, Category = "LauchPad")
	float LaunchStrenth;
	UPROPERTY(EditInstanceOnly, Category = "LauchPad")
	float LaunchPitchAngle;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* LaunchPadEffect;


};
