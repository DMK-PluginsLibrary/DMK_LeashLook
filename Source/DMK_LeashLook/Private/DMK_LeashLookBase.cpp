#include "DMK_LeashLookBase.h"

#include "DMK_LeashedControllerInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

#include "Kismet/KismetMathLibrary.h"


ADMK_LeashLookBase::ADMK_LeashLookBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADMK_LeashLookBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADMK_LeashLookBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsActive == false)
	{
		return;
	}
	DetermineLeashState();
	if (LeashState == ELeashState::InteractionWithoutLineSight)
	{
		AccumulatedDeltaTime = 0.f;
		if (CachedPlayerController)
		{
			ACharacter* PlayerCharacter = CachedPlayerController->GetCharacter();
			if (PlayerCharacter != nullptr && PlayerCharacter->Implements<UDMK_LeashedControllerInterface>())
			{
				IDMK_LeashedControllerInterface::Execute_SetLeashMouseInput(PlayerCharacter, 1.f);
			}
		}
	}
	else
	{
		AccumulatedDeltaTime += DeltaTime;
		AdjustPlayerInput(DeltaTime);
		if (CachedPlayerController)
		{
			ACharacter* PlayerCharacter = CachedPlayerController->GetCharacter();
			if (PlayerCharacter != nullptr && PlayerCharacter->Implements<UDMK_LeashedControllerInterface>())
			{
				USceneComponent* Camera = IDMK_LeashedControllerInterface::Execute_GetCameraComponent(PlayerCharacter);
				if (Camera != nullptr)
				{
					AddMouseInput(Camera->GetComponentLocation(),GetLeashOriginLocation(), DeltaTime);

				}
			}
		}
	}
}

void ADMK_LeashLookBase::DetermineLeashState_Implementation()
{
	if (CachedPlayerController == false || CachedPlayerController->GetCharacter() == nullptr)
	{
		return;
	}

	ACharacter* PlayerCharacter = CachedPlayerController->GetCharacter();
	if (PlayerCharacter->Implements<UDMK_LeashedControllerInterface>() == false)
	{
		return;
	}
	USceneComponent* CameraComponent = IDMK_LeashedControllerInterface::Execute_GetCameraComponent(PlayerCharacter);
	if(CameraComponent == nullptr)
	{
		return;
	}
	const float DotProduct = FVector::DotProduct(CameraComponent->GetForwardVector(),(GetLeashOriginLocation() - CameraComponent->GetComponentLocation()).GetSafeNormal());

	LeashState = DotProduct >= OriginDotAngle ? ELeashState::Origin : (DotProduct >= InnerDotAngle ? ELeashState::Inner : (DotProduct >= OuterDotAngle ? ELeashState::Outer : ELeashState::InteractionWithoutLineSight));
}

void ADMK_LeashLookBase::AdjustPlayerInput_Implementation(const float DeltaTime)
{
	if (CachedPlayerController == false || CachedPlayerController->GetCharacter() == nullptr)
	{
		return;
	}

	float* GoalPullStrength = PullStrengthMap.Find(LeashState);
	if (GoalPullStrength == nullptr)
	{
		return;
	}
	ACharacter* CachedPlayerCharacter = CachedPlayerController->GetCharacter();
	if (CachedPlayerCharacter->Implements<UDMK_LeashedControllerInterface>() == false)
	{
		return;
	}
	
	const float CurrentMultiplayer = IDMK_LeashedControllerInterface::Execute_GetLeashMouseInput(CachedPlayerCharacter);

	const float StrengthGrowthCurveValue = StrengthGrowthCurve.GetRichCurveConst()->Eval(AccumulatedDeltaTime);
	const float GoalMultiplayer = (*GoalPullStrength) - CurrentMultiplayer;
	const float CurrentStrengthPerSecond = ((*GoalPullStrength) - CurrentMultiplayer) > 0.f ? DefaultStrengthChangePerSecond : StrengthGrowthCurveValue;
	IDMK_LeashedControllerInterface::Execute_SetLeashMouseInput(CachedPlayerCharacter, CurrentMultiplayer + GoalMultiplayer * CurrentStrengthPerSecond * DeltaTime);
}

void ADMK_LeashLookBase::AddMouseInput_Implementation(const FVector& Start, const FVector& Target, float DeltaTime)
{
	if (CachedPlayerController == false)
	{
		return;
	}
	const FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	const FRotator PlayerControlRotation = CachedPlayerController->GetControlRotation();
	
	const float YawChange = CalculateInputToOrigin(DeltaRotation.Yaw, PlayerControlRotation.Yaw) * DeltaTime * PullSpeedPerSecond;
	CachedPlayerController->AddYawInput(YawChange);

	const float PitchChange = -CalculateInputToOrigin(DeltaRotation.Pitch, PlayerControlRotation.Pitch) * DeltaTime * PullSpeedPerSecond;
	CachedPlayerController->AddPitchInput(PitchChange);
	
}

float ADMK_LeashLookBase::CalculateInputToOrigin_Implementation(const float TargetRotationAxis,
                                                                const float CurrentRotationAxis)
{
	float DeltaRotation = FMath::ClampAngle(TargetRotationAxis - CurrentRotationAxis, -90.f, 90.f);
	
	DeltaRotation = FMath::Abs(DeltaRotation) > 2.f ? DeltaRotation : 0.f;
	DeltaRotation = FMath::Clamp(DeltaRotation, -PullSpeed, PullSpeed);\

	return DeltaRotation;
}

