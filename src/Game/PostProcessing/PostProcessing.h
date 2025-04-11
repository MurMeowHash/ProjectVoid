#pragma once

#include "../../Utils/Utils.h"

namespace PostProcessing {
    void SetGamma(float value);
    void SetExposure(float value);

    NODISCARD float GetGamma();
    NODISCARD float GetExposure();
}