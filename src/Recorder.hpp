#pragma once

#include "rack.hpp"
#include "Utils.hpp"

namespace lufu
{
    class RecorderModule;
    class RecorderWidget : public lufu::BaseModuleWidget<RecorderModule>
    {
    public:
        RecorderWidget(RecorderModule * module);
    };
}