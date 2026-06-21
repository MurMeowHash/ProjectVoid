#pragma once

#include "../../Utils/Utils.h"

namespace PostProcessing {
    void SetGamma(float value);
    void SetExposure(float value);
    void SetFogStart(float value);
    void SetFogEnd(float value);
    void SetFogColor(glm::vec3 value);
    void SetFogEnabled(bool value);

    NODISCARD float GetGamma();
    NODISCARD float GetExposure();
    NODISCARD float GetFogStart();
    NODISCARD float GetFogEnd();
    NODISCARD glm::vec3 GetFogColor();
    NODISCARD bool GetFogEnabled();
}