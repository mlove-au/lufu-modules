#pragma once
#include "rack.hpp"
#include <iostream>

namespace lufu
{

struct MulticlockModuleWidget : rack::ModuleWidget
{
    MulticlockModuleWidget();
};

// nicked from JWModules for now.
struct CenteredLabel : rack::Widget
{
    std::string text;
    int fontSize{16};
    std::shared_ptr<rack::Font> font_;
    CenteredLabel(int _fontSize = 16)
    {
        fontSize = _fontSize;
        box.size.y = BND_WIDGET_HEIGHT;
    }

    void setFont(std::shared_ptr<rack::Font> f)
    {
        font_ = f;
    }

    void draw(NVGcontext* vg) override
    {
        nvgTextAlign(vg, NVG_ALIGN_CENTER);
        nvgFillColor(vg, nvgRGB(10, 10, 10));
        nvgFontSize(vg, fontSize);
        if (font_)
        {
            nvgFontFaceId(vg, font_->handle);
        }
        nvgText(vg, box.pos.x, box.pos.y, text.c_str(), NULL);
    }
};

}