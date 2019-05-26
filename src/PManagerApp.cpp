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
    struct Mapping {
        int* readVal;
        Anim<float>* target;
    };

    std::unordered_map<std::string, Mapping> mParamMap;

    ch::CommsManager mCommsManager;
    std::unordered_map<int, int> mMidiBank;
    std::unordered_map<int, int> mMidiSeq;

    ch::PManager mPManager;
    std::vector<float> mPValues;

    std::vector<EaseFn> mAnimLookup = std::vector<EaseFn>{EaseInQuad()};
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
            [this] (int index, int value) { mMidiBank[index] = value; });

    mCommsManager.addListener("/midi/midi_fighter_twister/0/8/control_change",
            [this] (int index, int value) { mMidiSeq[index] = value; });

    mMidiBank[0] = 0;
    mMidiBank[1] = 0;
    mMidiBank[2] = 0;
    mMidiBank[3] = 0;

    mMidiSeq[0x0a] = 0x0;
    mParamMap["Circle.Radius"] = Mapping{&mMidiSeq[0x0a], &mCircleRadius};

    static Anim<float> sCircleAnimDuration = 0.1;
    mParamMap["Circle.Radius.Anim.Duration"] = Mapping{nullptr, &sCircleAnimDuration};

    static Anim<float> sCircleRadiusAnimIn = 0;
    mParamMap["Circle.Radius.Anim.In"] = Mapping{nullptr, &sCircleRadiusAnimIn};

    static Anim<float> sCircleRadiusAnimOut = 0;
    mParamMap["Circle.Radius.Anim.Out"] = Mapping{nullptr, &sCircleRadiusAnimOut};
}

void PManagerApp::update() {
    mPManager.update();
    mPManager.setSelected(lmap(mMidiBank[0], 0, 127, 0, 12));
    mCircleRadius = lmap(mMidiBank[1], 0, 127, 1, 100);
    mCircleOffset.ptr()->x = lmap(mMidiBank[2], 0, 127,
            -getWindowWidth() / 2, getWindowWidth() / 2);
    mCircleOffset.ptr()->y = lmap(mMidiBank[3], 0, 127,
            -getWindowHeight() / 2, getWindowHeight() / 2);

    const auto p = mParamMap["Circle.Radius"];
    if (*p.readVal != 0x0) {
        const auto radius = p.target->value();

        const auto q = mParamMap["Circle.Radius.Anim.Duration"];
        const auto duration = q.target->value();

        const auto r = mParamMap["Circle.Radius.Anim.In"];
        const auto animIn = mAnimLookup[r.target->value()];

        const auto s = mParamMap["Circle.Radius.Anim.Out"];
        const auto animOut = mAnimLookup[s.target->value()];

        timeline().apply(p.target, radius * (*p.readVal) / 64, duration, animIn);
        timeline().appendTo(p.target, radius, duration, animOut);

        *p.readVal = 0x0;  // reset impulse
    }
}

void PManagerApp::draw() {
    gl::clear(Color(0, 0, 0));

    gl::drawSolidCircle(getWindowCenter() + mCircleOffset.value(), mCircleRadius);
}

CINDER_APP(PManagerApp, RendererGl, PManagerApp::prepareSettings)
