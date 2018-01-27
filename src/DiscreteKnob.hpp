#pragma once

#include "rack.hpp"
#include <type_traits>

//
// Snaps values of a knob to _step_ size increments.
//
template <typename BaseKnob>
class DiscreteKnob : public BaseKnob
{
public:
    static_assert(std::is_base_of<rack::Knob, BaseKnob>::value,
                  "DiscreteKnob can only be applied to types derived from "
                  "'rack::Knob', sorry.");

    DiscreteKnob(float step = 0.25)
    : BaseKnob()
    , step_(step)
    {
    }

    void onDragMove(rack::EventDragMove& e) override
    {
        BaseKnob::onDragMove(e);
        BaseKnob::value = roundf(BaseKnob::value / step_) * step_;
    }

    const float step_;
};
