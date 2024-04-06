// HydroSchemes Simulator. All rights reserved.

#pragma once

#include "Tests/Utils/InputRecordingUtils.h"
#include "CoreMinimal.h"

namespace Test {
class JsonUtils
{
public:
	static bool WriteInputData(const FString& FileName, const FInputData& InputData);
	static bool ReadInputData(const FString& FileName, FInputData& InputData);
};
}
