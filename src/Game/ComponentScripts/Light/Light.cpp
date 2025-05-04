#include "Light.h"

Light::Light(const LightParameters &params)
: ObjectComponent(ENGINE_COMPONENTS_START_PRIORITY + 3), type(params.type), color(params.color), intensity(params.intensity),
radius(params.radius), innerCutOff(params.innerCutOff), outerCutOff(params.outerCutOff) {

}

void Light::Update() {

}
