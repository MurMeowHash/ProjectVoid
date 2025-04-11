#include "Light.h"

Light::Light(const LightParameters &params)
: ObjectComponent(2), type(params.type), color(params.color), intensity(params.intensity),
radius(params.radius), innerCutOff(params.innerCutOff), outerCutOff(params.outerCutOff) {

}

void Light::Update() {

}
