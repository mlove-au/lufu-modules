#include "Multiclock.hpp"
#include "ClockDSP.hpp"
#include "LabelledKnob.hpp"
#include "catch.hpp"
#include "dsp/digital.hpp"
#include <iostream>

using namespace rack;

namespace lufu
{

    class MultiClockModule : public rack::Module
    {
        public:
            enum ParamIds
            {
                BPM_PARAM,
                ON_OFF_PARAM,
                NUM_PARAMS
            };
            enum InputIds
            {
                NUM_INPUTS
            };

            enum OutputIds
            {
                CLOCK_OUTPUT,
                DOUBLE_CLOCK_OUTPUT,
                QUAD_CLOCK_OUTPUT,
                OCT_CLOCK_OUTPUT,
                ON_START_OUTPUT,
                NUM_OUTPUTS
            };

            enum LightIds
            {
                NUM_LIGHTS
            };

            MultiClockModule()
                : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
            {
            }

            void step() override;
            rack::PulseGenerator start_pulse_;
            bool running_;
            MultiClock<4> clock_;
    };

    void MultiClockModule::step()
    {
        clock_.set_bpm(params[BPM_PARAM].value);

        const double deltaTime = 1.0 / engineGetSampleRate();
        clock_.tick(deltaTime);
        if (params[ON_OFF_PARAM].value >= 1.0)
        {
            if (!running_)
            {
                running_ = true;
                start_pulse_.trigger(1e-3);
            }

            outputs[ON_START_OUTPUT].value = start_pulse_.process(deltaTime) ? 10.0f : 0.0f;
            outputs[CLOCK_OUTPUT].value = clock_.output<0>();
            outputs[DOUBLE_CLOCK_OUTPUT].value = clock_.output<1>();
            outputs[QUAD_CLOCK_OUTPUT].value = clock_.output<2>();
            outputs[OCT_CLOCK_OUTPUT].value = clock_.output<3>();
        }
        else
        {
            running_ = false;
            outputs[ON_START_OUTPUT].value = 0.0f;
            outputs[CLOCK_OUTPUT].value = 0.0f;
            outputs[DOUBLE_CLOCK_OUTPUT].value = 0.0f;
            outputs[QUAD_CLOCK_OUTPUT].value = 0.0f;
            outputs[OCT_CLOCK_OUTPUT].value = 0.0f;
        }
    }

    MultiClockModuleWidget::MultiClockModuleWidget(MultiClockModule * module)
        : BaseModuleWidget(module, "res/Clock.svg")
    {
        addChild(createParam<rack::NKK>(Vec(32, 48), module, MultiClockModule::ON_OFF_PARAM, 0.0, 1.0, 1.0));

        using BPMKnob = LabelledKnob<RoundBlackKnob>;
        auto bpm_knob = dynamic_cast<BPMKnob*>(createParam<BPMKnob>(Vec(28, 95), module, MultiClockModule::BPM_PARAM, 0.0, 500, 120.0));
        addParam(bpm_knob);

        auto* const bpmLabel = new rack::Label;
        bpmLabel->box.pos = Vec(20, 136);
        bpmLabel->text = "";
        bpm_knob->setLabel(bpmLabel, [](float v) { return std::to_string(int(v)) + " BPM"; });
        addChild(bpmLabel);

        addOutput(createOutput<CL1362Port>(Vec(30, 158), module, MultiClockModule::ON_START_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 200), module, MultiClockModule::CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 235), module, MultiClockModule::DOUBLE_CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 270), module, MultiClockModule::QUAD_CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 305), module, MultiClockModule::OCT_CLOCK_OUTPUT));
    }

    Model* g_multiclockModel = Model::create<lufu::MultiClockModule, lufu::MultiClockModuleWidget>(
        "lufu", "Multiclock", "Multiclock", rack::UTILITY_TAG);
}
