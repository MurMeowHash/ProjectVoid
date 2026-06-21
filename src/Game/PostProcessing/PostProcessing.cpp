#include "PostProcessing.h"

namespace PostProcessing {
    constexpr float DEFAULT_SCREEN_GAMMA = 2.2f;

    float screenGamma = DEFAULT_SCREEN_GAMMA;
    float exposure = 10.0f;
    float fogStart = 0.0f;
    float fogEnd = 50.0f;
    glm::vec3 fogColor = glm::vec3(0.05f);
    bool fogEnabled = true;

    void SetGamma(float value) {
        if(value < 0.0f) return;

        screenGamma = value;
    }

    void SetExposure(float value) {
        if(value < 0.0f) return;

        exposure = value;
    }

    void SetFogStart(float value) {
        if (value < 0.0f) return;

        fogStart = value;
    }

    void SetFogEnd(float value) {
        if (value < fogStart) return;

        fogEnd = value;
    }

    void SetFogColor(glm::vec3 value) {
        fogColor = value;
    }

    void SetFogEnabled(bool value) {
        fogEnabled = value;
    }

    float GetGamma() {
        return screenGamma;
    }

    float GetExposure() {
        return exposure;
    }

    float GetFogStart() {
        return fogStart;
    }

    float GetFogEnd() {
        return fogEnd;
    }

    glm::vec3 GetFogColor() {
        return fogColor;
    }

    bool GetFogEnabled() {
        return fogEnabled;
    }
}
