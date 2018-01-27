#pragma once
#include "rack.hpp"
#include <dsp/digital.hpp>
#include <iostream>

extern rack::Plugin* plugin;
using namespace rack;

class DeltaWaveDSP
{
private:
    float prev_{0.0};
    float d1_{0.0};
    rack::PulseGenerator output_;
    bool signal_{false};

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
   float dt_{0.0};


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

struct DeltaWaveWidget : rack::ModuleWidget
{

    std::shared_ptr<Font> font_;

    DeltaWaveWidget()
    {
        font_ = Font::load(assetPlugin(plugin, "res/Hanken-Light.ttf"));

        DeltaWaveModule* module = new DeltaWaveModule();
        setModule(module);
        box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        SVGPanel* panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/MyModule.svg")));
        addChild(panel);

        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        CenteredLabel* const title = new CenteredLabel;
        title->box.pos = Vec(23, 20);
        title->text = "Δ Trigger";
        //  title->setFont(font_);
        addChild(title);

        addChild(createParam<rack::NKK>(Vec(32, 48), module, DeltaWaveModule::ON_OFF_PARAM, 0.0, 1.0, 1.0));

        addInput(createInput<PJ301MPort>(Vec(33, 163), module, DeltaWaveModule::INPUT_1));
        addOutput(createOutput<CL1362Port>(Vec(30, 253), module, DeltaWaveModule::CLOCK_OUTPUT));
    }
};
