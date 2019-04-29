#pragma once

#include "rack.hpp"
#include "Utils.hpp"

namespace lufu
{
    class Repeater;
    class RepeaterWidget : public lufu::BaseModuleWidget<Repeater>
    {
        public:
            RepeaterWidget(Repeater * module);
    };
}