
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DMK_LeashedControllerInterface.generated.h"

UINTERFACE(BlueprintType)
class UDMK_LeashedControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface is for mouse interface when we are leashed by plugin
 */
class DMK_LEASHLOOK_API IDMK_LeashedControllerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetLeashMouseInput(const float GoalYawMultiplicator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetLeashMouseInput();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USceneComponent* GetCameraComponent();
};
