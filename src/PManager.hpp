// PManager.hpp
// Callum Howard, 2019

#ifndef PMANAGER_HPP
#define PMANAGER_HPP

#include <vector>
#include <string>

#include "cinder/app/App.h"

#include "Parameter.hpp"


namespace ch {
using namespace ci;
using namespace ci::app;

class PManager {
public:
    void setup();
    void update();
    PManager& addParameter(const std::string& name, float* value);
    PManager& addParameter(const std::string& name, float* value, float min, float max);

private:
    static const bool sHiDpi = true;
    ImFont* font1;
    float mNum = 0.0;
    using PVar = Parameter*;
    std::vector<PVar> mParams;
};

void PManager::setup() {
    // create the renderer and initialize the ui resources.
    //const auto fontName = "Roboto-Medium.ttf";
    //auto ttfFont = getAssetPath(fontName);

    const auto options = ui::Options()
        //.font(ttfFont, 12)
        //.fontGlyphRanges(fontName, ui::GetIO().Fonts->GetGlyphRangesDefault())
        .windowRounding(0.0f);
    ui::initialize(options);

    if (sHiDpi) {
        //ui::GetStyle().ScaleAllSizes(getWindowContentScale());
        auto& style = ui::GetStyle();
        float scale_factor = 2.0f;
        //style.WindowPadding *= ImVec2{2, 2};
        //style.WindowMinSize *= scale_factor;
        style.WindowRounding *= scale_factor;
        style.ChildWindowRounding *= scale_factor;
        //style.FramePadding *= scale_factor;
        style.FrameRounding *= scale_factor;
        //style.ItemSpacing *= scale_factor;
        //style.ItemInnerSpacing *= scale_factor;
        //style.TouchExtraPadding *= scale_factor;
        style.IndentSpacing *= scale_factor;
        style.ColumnsMinSpacing *= scale_factor;
        style.ScrollbarSize *= scale_factor;
        style.ScrollbarRounding *= scale_factor;
        style.GrabMinSize *= scale_factor;
        style.GrabRounding *= scale_factor;
        //style.DisplayWindowPadding *= scale_factor;
        //style.DisplaySafeAreaPadding *= scale_factor;

        ui::GetIO().FontGlobalScale = getWindowContentScale();
    }
}

void PManager::update() {
    ui::SetNextWindowPos(ivec2{0, 0});
    ui::SetNextWindowSize(getWindowSize() * (sHiDpi ? 2 : 1));
    ui::ScopedWindow window{"Parameter Manager", ImGuiWindowFlags_NoTitleBar};

    static int selected = 0;

    if (not mParams.empty()) {
        auto p = static_cast<ParameterFloat*>(mParams.at(selected));
        // slider
        ui::SliderFloat("Number", p->valuePointer, p->pMin, p->pMax);
        ui::RangeSliderFloat("Range", &p->min, &p->max, p->pMin, p->pMax);
        // animation curve
        // animation speed
    }

    ui::Spacing();

    int i = 0;
    for (auto& param : mParams) {
        auto p = static_cast<ParameterFloat*>(param);
        auto id = "##" + std::to_string(i);  // after ## is not visible
        ui::RadioButton(id.c_str(), &selected, i);
        ui::SameLine();
        ui::SliderFloat(p->name.c_str(), p->valuePointer, p->min, p->max);
        ++i;
    }
}

PManager& PManager::addParameter(const std::string& name, float* valueAddress) {
    mParams.push_back(static_cast<PVar>(new ParameterFloat{name, valueAddress}));
    return *this;
}

PManager &PManager::addParameter(
        const std::string &name, float *valueAddress, float min, float max) {
    mParams.push_back(
            static_cast<PVar>(new ParameterFloat{name, valueAddress, min, max}));
    return *this;
}

} // namespace ch

#endif
