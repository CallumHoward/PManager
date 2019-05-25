// PManagerApp.cpp
// Callum Howard, 2019

#include <iterator>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"          // constrain, lmap
#include "cinder/Timeline.h"

#include "CinderImGui.h"

#include "CommsManager.hpp"
#include "PManager.hpp"

using namespace ci;
using namespace ci::app;

class PManagerApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    static void prepareSettings(PManagerApp::Settings *settings) {
        settings->setHighDensityDisplayEnabled();
    }

private:
    ch::PManager mPManager;
    std::vector<float> mPValues;

    ch::CommsManager mCommsManager;
    std::unordered_map<int, int> mParamMappings;

    Anim<float> mCircleRadius = 25.0f;
    Anim<vec2> mCircleOffset = vec2{0, 0};
};

void PManagerApp::setup() {
    const auto pNames = std::vector<std::string>{"Foo", "Bar", "Baz", "Fizz", "Buzz", "Zap"};
    mPValues = std::vector<float>(pNames.size(), 0.5f);

    mPManager.setup();
    for (size_t i = 0; i < pNames.size(); ++i) {
        mPManager.addParameter(pNames[i], &mPValues[i]);
    }

    mPManager.addParameter("Circle Radius", mCircleRadius.ptr(), 1.0f, 100.0f);
    mPManager.addParameter("Circle Offset X", &(mCircleOffset.ptr()->x),
            -getWindowWidth() / 2, getWindowWidth() / 2);
    mPManager.addParameter("Circle Offset Y", &(mCircleOffset.ptr()->y),
            -getWindowHeight() / 2, getWindowHeight() / 2);

    // OSC
    mCommsManager.setup("/midi/midi_fighter_twister/0/1/control_change",
            [this] (int index, int value) { mParamMappings[index] = value; });

    mCommsManager.addListener("/midi/midi_fighter_twister/0/8/control_change",
            [this] (int index, int value) { mParamMappings[index] = value; });

    mParamMappings[0] = 0;
    mParamMappings[1] = 0;
    mParamMappings[2] = 0;
    mParamMappings[3] = 0;
    mParamMappings[0x0a] = 0x0;
}

void PManagerApp::update() {
    mPManager.update();
    mPManager.setSelected(lmap(mParamMappings[0], 0, 127, 0, 12));
    mCircleRadius = lmap(mParamMappings[1], 0, 127, 1, 100);
    mCircleOffset.ptr()->x = lmap(mParamMappings[2], 0, 127,
            -getWindowWidth() / 2, getWindowWidth() / 2);
    mCircleOffset.ptr()->y = lmap(mParamMappings[3], 0, 127,
            -getWindowHeight() / 2, getWindowHeight() / 2);

    CI_LOG_I(mParamMappings[0x0a]);

    if (mParamMappings[0x0a] != 0x0) {
        const auto radius = mCircleRadius.value();
        timeline().apply(&mCircleRadius, radius * mParamMappings[0x0a] / 64, 0.1f, EaseInQuad());
        timeline().appendTo(&mCircleRadius, radius, 0.1f, EaseInQuad());
        mParamMappings[0x0a] = 0x0;
    }
}

void PManagerApp::draw() {
    gl::clear(Color(0, 0, 0));

    gl::drawSolidCircle(getWindowCenter() + mCircleOffset.value(), mCircleRadius);
}

CINDER_APP(PManagerApp, RendererGl, PManagerApp::prepareSettings)
