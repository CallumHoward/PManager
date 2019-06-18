// SuperFormula.hpp
// Callum Howard, 2019

#ifndef SUPERFORMULA_HPP
#define SUPERFORMULA_HPP

#include <cmath>

#include "cinder/gl/gl.h"

namespace ch {
using namespace ci;

struct SuperFormula {
    void setup();
    void update();
    void draw();

    float theta;
    float a;
    float b;
    float m;
    float n1;
    float n2;
    float n3;

    float r(float theta, float a, float b, float m, float n1, float n2, float n3) {
        return std::powf(
                std::powf(std::fabs(std::cosf(m * theta / 4.0f) / a), n2) +
                std::powf(std::fabs(std::sinf(m * theta / 4.0f) / b), n3), -1.0f / n1);
    }

    float mScaleFactor = 50.0f;
    float mTimeDelayFactor = 0.0f;
    const float mTimeFactor = 10.0f;
    const float mResolutionFactor = 1000.0f;
};

void SuperFormula::setup() {
    a = 1.0f;  // size
    b = 1.0f;
    m = 0.0f;            // number of rotational symmetry
    n1 = 1.0f;
    n2 = 1.0f; //sin(t / timeScaleFactor);  // shape of spikes
    n3 = 1.0f; //cos(t / timeScaleFactor);  // shape of spikes
}

void SuperFormula::update() {

}

void SuperFormula::draw() {
    gl::ScopedModelMatrix modelMatrix{};
    const auto center = getWindowCenter();
    gl::translate(center);

    for (float theta = 0.0f; theta <= 2.0f * M_PI; theta += 1.0f / mResolutionFactor) {
        float rad = r(theta, a, b, std::roundf(m), n1, n2, n3);
        float x = rad * std::cosf(theta) * mScaleFactor;
        float y = rad * std::sinf(theta) * mScaleFactor;
        // vertex
        gl::ScopedColor scopedColor{(center.x - x * 2) / center.x, (center.y - y * 2) / center.y, 1};
        gl::drawSolidCircle(vec2{x, y}, 1.0f);
    }
}



} // namespace ch

#endif
