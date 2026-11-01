#include "Animation.h"
#include <cmath>

float Animation::getWalkBob(float time, float speed, float amplitude) {
    return std::sin(time * speed) * amplitude;
}

float Animation::getLimbSwing(float time, float speed, float maxAngle) {
    return std::sin(time * speed) * maxAngle;
}
