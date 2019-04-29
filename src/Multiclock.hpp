#pragma once
#include "rack.hpp"
#include "Utils.hpp"

namespace lufu
{
    class MultiClockModule;

    class MultiClockModuleWidget : public lufu::BaseModuleWidget<MultiClockModule>
    {
    public:
        MultiClockModuleWidget(MultiClockModule * module);
    };

    extern rack::Model * g_multiclockModel;
}