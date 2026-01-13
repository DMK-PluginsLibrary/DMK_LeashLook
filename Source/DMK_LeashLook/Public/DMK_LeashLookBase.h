#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMK_LeashLookBase.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class ELeashState : uint8
{
	NoInteraction,
	InteractionWithoutLineSight,
	Outer,
	Inner,
	Origin,
};

UCLASS(Blueprintable, BlueprintType)
class DMK_LEASHLOOK_API ADMK_LeashLookBase : public AActor
{
	GENERATED_BODY()

public:
	ADMK_LeashLookBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float CalculateInputToOrigin(const float TargetRotationAxis, const float CurrentRotationAxis);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddMouseInput(const FVector& Start, const FVector& Target, float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AdjustPlayerInput(const float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DetermineLeashState();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	FVector GetLeashOriginLocation() const;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> CachedPlayerController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(NoElementDuplicate))
	TMap<ELeashState, float> PullStrengthMap = {
		{ELeashState::NoInteraction, 0.f}, {ELeashState::InteractionWithoutLineSight, 0.f}, {ELeashState::Outer, 0.1f}, {ELeashState::Inner, 0.05f}, {ELeashState::Origin, 0.f}
	};
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PullSpeed = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PullSpeedPerSecond = 25.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELeashState LeashState = ELeashState::NoInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccumulatedDeltaTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultStrengthChangePerSecond = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRuntimeFloatCurve StrengthGrowthCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OriginDotAngle = 0.96f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InnerDotAngle = 0.85f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OuterDotAngle = 0.65f;
};
