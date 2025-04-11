#include "PostProcessing.h"

namespace PostProcessing {
    constexpr float DEFAULT_SCREEN_GAMMA = 2.2f;

    float screenGamma = DEFAULT_SCREEN_GAMMA;
    float exposure = 1.0f;

    void SetGamma(float value) {
        if(value < 0.0f) return;

        screenGamma = value;
    }

    void SetExposure(float value) {
        if(value < 0.0f) return;

        exposure = value;
    }

    float GetGamma() {
        return screenGamma;
    }

    float GetExposure() {
        return exposure;
    }
}