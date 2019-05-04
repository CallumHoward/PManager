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
    static void prepareSettings(PManagerApp::Settings *settings) {
        settings->setHighDensityDisplayEnabled();
    }

private:
    static const bool sHiDpi = true;
    ImFont* font1;
    float mNum = 0.0;
    boost::container::flat_map<string, float> mParams;
};

void PManagerApp::setup() {
    mParams["Foo"] = 0.0f;
    mParams["Bar"] = 0.0f;
    mParams["Baz"] = 0.0f;
    mParams["Buz"] = 0.0f;

    // create the renderer and initialize the ui resources.
    const auto fontName = "Roboto-Medium.ttf";
    auto ttfFont = getAssetPath(fontName);

    const auto options = ui::Options()
        .font(ttfFont, 24)
        //.fontGlyphRanges(fontName, ui::GetIO().Fonts->GetGlyphRangesDefault())
        .windowRounding(0.0f);
    ui::initialize(options);

    //auto io = ui::GetIO();
    ////font1 = io.Fonts->AddFontFromFileTTF(ttfFont.c_str(), 24);
    ////io.Fonts->GetTexDataAsRGBA32();
    //font1 = io.Fonts->AddFontDefault();
    //assert(font1 != NULL);
    //ui::PushFont(font1);
}

void PManagerApp::mouseDown(MouseEvent event) {}

void PManagerApp::update() {

    ui::SetNextWindowPos(ivec2{0, 0});
    ui::SetNextWindowSize(getWindowSize() * (sHiDpi ? 2 : 1));
    ui::ScopedWindow window{"Parameter Manager", ImGuiWindowFlags_NoTitleBar};

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
        auto id = "##" + std::to_string(i);  // after ## is not visible
        ui::RadioButton(id.c_str(), &selected, i);
        ui::SameLine();
        ui::SliderFloat(pKey.c_str(), &pValue, 0.0f, 1.0f);
        ++i;
    }
}

void PManagerApp::draw() { gl::clear(Color(0, 0, 0)); }

CINDER_APP(PManagerApp, RendererGl, PManagerApp::prepareSettings)
