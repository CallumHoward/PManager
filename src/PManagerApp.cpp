#include <boost/container/flat_map.hpp>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PManagerApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;

private:
    float mNum = 0.0;
    boost::container::flat_map<string, float> mParams;
};

void PManagerApp::setup() {
    mParams["Foo"] = 0.0f;
    mParams["Bar"] = 0.0f;
    mParams["Baz"] = 0.0f;
    mParams["Buz"] = 0.0f;

    // create the renderer and initialize the ui resources.
    ui::initialize(); //TODO ui::Options change font
}

void PManagerApp::mouseDown(MouseEvent event) {}

void PManagerApp::update() {
    // slider
    ui::SliderFloat("Number", &mNum, 0.01f, 0.0f);
    // min max
    //ui::DragFloat2("Min/Max", 

    // animation curve
    // animation speed

    ui::Spacing();

    static int selected = 0;
    ui::SliderInt("Selected", &selected, 0, mParams.size() - 1);

    int i = 0;
    for (auto& param : mParams) {
        auto pKey = param.first;
        auto pValue = param.second;
        ui::RadioButton(std::to_string(i).c_str(), &selected, i);
        ui::SameLine();
        ui::SliderFloat(pKey.c_str(), &pValue, 0.0f, 1.0f);
        ++i;
    }

}

void PManagerApp::draw() { gl::clear(Color(0, 0, 0)); }

CINDER_APP(PManagerApp, RendererGl)
