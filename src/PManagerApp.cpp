// PManagerApp.cpp
// Callum Howard, 2019

#include <iterator>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

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
};

void PManagerApp::setup() {
    const auto pNames = std::vector<std::string>{"Foo", "Bar", "Baz", "Fizz", "Buzz", "Zap"};
    mPValues = std::vector<float>(pNames.size(), 0.5f);

    mPManager.setup();
    for (size_t i = 0; i < pNames.size(); ++i) {
        mPManager.addParameter(pNames[i], &mPValues[i]);
    }
}

void PManagerApp::update() {
    mPManager.update();
}

void PManagerApp::draw() { gl::clear(Color(0, 0, 0)); }

CINDER_APP(PManagerApp, RendererGl, PManagerApp::prepareSettings)
