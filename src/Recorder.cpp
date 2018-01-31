#include "rack.hpp"
#include "Recorder.hpp"
#include "WavWriter.hpp"


namespace lufu
{
    class RecorderModule : public rack::Module
    {
        enum ParamIds
        {
            NUM_PARAMS
        };
        enum InputIds
        {
            INPUT_L,
            INPUT_R,
            NUM_INPUTS
        };

        enum OutputIds
        {
            NUM_OUTPUTS
        };

        enum LightIds
        {
            NUM_LIGHTS
        };

    public:
        RecorderModule()
            : rack::Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
        {
        }
    };

    extern rack::Plugin * plugin;

    RecorderWidget::RecorderWidget()
    {
     
        auto module = new RecorderModule();
        setModule(module);
        box.size = rack::Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            auto panel = new rack::SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(rack::SVG::load(assetPlugin(plugin, "res/Recorder.svg")));
            addChild(panel);
        }
        /*
        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addChild(createParam<rack::NKK>(Vec(32, 48), module, MultiClockModule::ON_OFF_PARAM, 0.0, 1.0, 1.0));

        using BPMKnob = LabelledKnob<RoundBlackKnob>;
        auto bpm_knob = dynamic_cast<BPMKnob*>(createParam<BPMKnob>(Vec(28, 95), module, MultiClockModule::BPM_PARAM, 0.0, 500, 120.0));
        addParam(bpm_knob);

        auto* const bpmLabel = new rack::Label;
        bpmLabel->box.pos = Vec(20, 136);
        bpmLabel->text = "";
        bpm_knob->setLabel(bpmLabel, [](float v) { return std::to_string(int(v)) + " BPM"; });
        addChild(bpmLabel);

        addOutput(createOutput<CL1362Port>(Vec(30, 163), module, MultiClockModule::CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 213), module, MultiClockModule::DOUBLE_CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 263), module, MultiClockModule::QUAD_CLOCK_OUTPUT));
        addOutput(createOutput<CL1362Port>(Vec(30, 313), module, MultiClockModule::OCT_CLOCK_OUTPUT));
        */
    };
}

