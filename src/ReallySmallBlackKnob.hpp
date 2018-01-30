#pragma once

#include "rack.hpp"

struct ReallySmallBlackKnob : RoundBlackKnob
{
    ReallySmallBlackKnob()
    {
        box.size = Vec(21, 21);
    }
};