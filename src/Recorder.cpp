#include "rack.hpp"
#include "Recorder.hpp"
#include "WavWriter.hpp"
#include "OpenFileButton.hpp"
#include "Utils.hpp"
#include <iostream>

namespace lufu
{

    class RecorderModule : public rack::Module
    {
    public:
        enum ParamIds
        {
            RECORD_STOP_BUTTON,
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

        void on_set_target_file(std::string path)
        {
            target_file_ = std::move(path);
            std::cerr << "Saving to filename " << target_file_ << "\n";
        }

      
        void step() override
        {
            if (params[RECORD_STOP_BUTTON].value == 1 && !sink_ && !target_file_.empty())
            {
                sink_ = std::unique_ptr<WavSink>(new WavSink(target_file_, 44100));
            }

            if (params[RECORD_STOP_BUTTON].value == 0 && sink_)
            {
                sink_.reset();
            }

            if (sink_)
            {
                float left = inputs[INPUT_L].active ? inputs[INPUT_L].value : 0.0;
                float right = inputs[INPUT_R].active ? inputs[INPUT_R].value : 0.0;
                sink_->push_samples(left, right);
            }
        }
        
    private:
        std::string target_file_;
        std::unique_ptr<lufu::WavSink> sink_;
    };

    
    // Adjust position so that is centered horizontally in outer
    void center_horiz(const rack::Widget& other, rack::Widget & that)
    {
        auto center = other.box.size.x / 2;
        auto width = that.box.size.x / 2;

        that.box.pos.x = center - width;

        std::cerr << "Other center = " << center << "\n";
        std::cerr << "Width = " << width << "\n";
        std::cerr << "x Set to " << that.box.pos.x << "\n";
    }

    RecorderWidget::RecorderWidget()
    {
        using namespace rack;
        extern rack::Plugin * plugin;

        auto module = new RecorderModule();
        setModule(module);
        box.size = rack::Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            auto panel = new rack::SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(rack::SVG::load(assetPlugin(plugin, "res/Recorder.svg")));
            addChild(panel);
        }
        
        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        // should be a button, red LED, swquare.
        auto rec = createParam<rack::NKK>(Vec(32, 48), module, RecorderModule::RECORD_STOP_BUTTON, 0.0, 1.0, 1.0);
        center_horiz(*this, *rec);
        addParam(rec);

        auto open_file = new OpenFileButton([module](const std::string& path)
        {
            module->on_set_target_file(path);
        }, OSDIALOG_SAVE);

        open_file->box.pos = Vec(40, 90);

        center_horiz(*this, *open_file);
        addChild(open_file);

        addInput(createInput<PJ301MPort>(Vec(10, 310), module, RecorderModule::INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(50, 310), module, RecorderModule::INPUT_R));

        /*
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
        */
    };
}

