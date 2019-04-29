#pragma once
#include "rack.hpp"
#include <dsp/digital.hpp>
#include <iostream>
#include "Utils.hpp"

using namespace rack;

namespace lufu
{

    class DeltaWaveDSP
    {
    private:
        float prev_{ 0.0 };
        float d1_{ 0.0 };
        rack::PulseGenerator output_;
        bool signal_{ false };

    public:
        void tick(float dt, float input)
        {
            const auto d1 = input - prev_;
            if ((d1 > 0.0 && d1_ <= 0.0) || (d1 < 0.0 && d1_ >= 0.0))
            {
                output_.trigger(1e-3);
            }
            signal_ = output_.process(dt);
            d1_ = d1;
            prev_ = input;
        }

        float output() const
        {
            return signal_ ? 10.0 : 0.0;
        }
    };

    struct DeltaWaveModule : rack::Module
    {
        enum ParamIds
        {
            ON_OFF_PARAM,
            NUM_PARAMS
        };

        enum InputIds
        {
            INPUT_1,
            NUM_INPUTS
        };

        enum OutputIds
        {
            CLOCK_OUTPUT,
            NUM_OUTPUTS
        };

        enum LightIds
        {
            NUM_LIGHTS
        };


        DeltaWaveDSP dsp_;
        float dt_{ 0.0 };


        DeltaWaveModule()
            : rack::Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
        {
            dt_ = 1.0f / rack::engineGetSampleRate();
        }

        void onSampleRateChange() override
        {

            dt_ = 1.0f / rack::engineGetSampleRate();
        }

        void step() override
        {
            dsp_.tick(dt_, inputs[INPUT_1].value);
            if (params[ON_OFF_PARAM].value == 1.0)
            {
                outputs[CLOCK_OUTPUT].value = dsp_.output();
            }
            else
            {
                outputs[CLOCK_OUTPUT].value = 0.0;
            }
        }
    };

    struct DeltaWaveWidget : lufu::BaseModuleWidget<DeltaWaveModule>
    {
        DeltaWaveWidget(DeltaWaveModule * module)
            : BaseModuleWidget(module, "res/Delta.svg")
        {     
            addChild(createParam<rack::NKK>(Vec(32, 48), module, DeltaWaveModule::ON_OFF_PARAM, 0.0, 1.0, 1.0));
            addInput(createInput<PJ301MPort>(Vec(33, 163), module, DeltaWaveModule::INPUT_1));
            addOutput(createOutput<CL1362Port>(Vec(30, 253), module, DeltaWaveModule::CLOCK_OUTPUT));
        }
    };

}