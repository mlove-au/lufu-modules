#pragma once

#include <type_traits>
#include <numeric>
#include <cmath>
#include <math.h>
#include <iostream>

namespace lufu
{
    class Oscillator
    {
    public:
        Oscillator(float freq, float amp)
        : freq_(freq) 
        , amp_(amp)
        , time_(0.0f)
        {
        }

        // freq in hz
        void freq(float f)
        {
            freq_ = f;
        }

        void amp(float a) 
        {
            amp_ = a;
        }

        void tick(float dt)
        {
            time_ += dt;
        }

        float time()
        {
            return time_;
        }

        float sine() const
        {
            constexpr auto PI_2 = 2.0 * M_PI;
            return ::sin(freq_ * PI_2 * time_);
        }
    private:
        double freq_;
        double amp_;
        double time_;
    };
}