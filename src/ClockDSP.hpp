#pragma once

#include "dsp/digital.hpp"
#include <array>

struct Clock
{
    Clock()
    : bps_(0.0)
    {
        
    }

    void tick(double dt)
    {
        const auto inc = dt * bps_;
        phase_ += inc;

        if (clock_trigger_1_.process(phase_))
        {
            clock_1_.trigger(1e-3);
        }

        t1_ = clock_1_.process(dt);

        if (phase_ >= 1.0)
        {
            phase_ -= 1.0;
        }
    }

    void set_bpm(double bpm)
    {
        bps_ = bpm / 60.0;
    }

    double clock_out()
    {
        return t1_ ? 10.0 : 0.0;
    };

    bool t1_{false};
    rack::SchmittTrigger clock_trigger_1_;
    rack::PulseGenerator clock_1_;
    double bps_{0.0};
    double phase_{0.0};
};

template <size_t multipiers = 4>
struct MultiClock
{
    std::array<Clock, multipiers> clocks_;

    void tick(double dt)
    {
        for (auto& c : clocks_)
        {
            c.tick(dt);
        }
    }

    template <size_t idx>
    double output()
    {
        static_assert(idx >= 0 && idx < multipiers, "index would be out of range");
        return clocks_[idx].clock_out();
    }

    void set_bpm(double bpm)
    {
        double m = 1.0;
        for (auto& c : clocks_)
        {
            c.set_bpm(bpm * m);
            m *= 2.0;
        }
    }
};
