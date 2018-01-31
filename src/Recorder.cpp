#include "rack.hpp"
#include "Recorder.hpp"
#include "WavWriter.hpp"
#include "OpenFileButton.hpp"
#include "Utils.hpp"
#include <iostream>
#include "dsp/vumeter.hpp"

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

        static constexpr int VU_METER_LIGHTS = 16;

        enum LightIds
        {
            VU_METER_LEFT_1,
            VU_METER_LEFT_END = VU_METER_LEFT_1 + VU_METER_LIGHTS,
            VU_METER_RIGHT_1,
            VU_METER_RIGHT_END = VU_METER_RIGHT_1 + VU_METER_LIGHTS,
            NUM_LIGHTS
        };

    public:
        RecorderModule()
            : rack::Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
        {
            meter_.dBInterval = 3;
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

            float left = inputs[INPUT_L].active ? inputs[INPUT_L].value : 0.0;
            float right = inputs[INPUT_R].active ? inputs[INPUT_R].value : 0.0;

            if (sink_)
            {
                sink_->push_samples(left, right);
            }
               
            meter_.setValue(left / 5.0);
            for (int l = 0; l < VU_METER_LIGHTS; l++)
            {
                lights[l + VU_METER_LEFT_1].setBrightnessSmooth(meter_.getBrightness(VU_METER_LIGHTS - l));                            
            }


            meter_.setValue(right / 5.0);
            for (int l = 0; l < VU_METER_LIGHTS; l++)
            {
                lights[l + VU_METER_RIGHT_1].setBrightnessSmooth(meter_.getBrightness(VU_METER_LIGHTS - l));
            }

            
        }
    private:
        std::string target_file_;
        std::unique_ptr<lufu::WavSink> sink_;
        rack::VUMeter meter_;
    };

    
    // Adjust position so that is centered horizontally in outer
    void center_horiz(const rack::Widget& other, rack::Widget & that)
    {
        auto center = other.box.size.x / 2;
        auto width = that.box.size.x / 2;

        that.box.pos.x = center - width;
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

        open_file->box.pos = Vec(40, 96);

        center_horiz(*this, *open_file);
        addChild(open_file);

        addInput(createInput<PJ301MPort>(Vec(10, 310), module, RecorderModule::INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(50, 310), module, RecorderModule::INPUT_R));


        for (int i = 0; i < RecorderModule::VU_METER_LIGHTS; i++)
        {
            if (i < 10)
            {
                std::cerr << 290 - i * 10 << "\n";
                auto left = createLight<rack::GreenLight>(Vec(16, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i);
                left->box.size = Vec(8, 8);
                addChild(left);
 
                auto right = createLight<rack::GreenLight>(Vec(56, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i);
                right->box.size = Vec(8, 8);
                addChild(right);
            }

            if (i >= 10 && i < 14)
            {
                std::cerr << 290 - i * 10 << "\n";
                auto left = createLight<rack::YellowLight>(Vec(16, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i);
                left->box.size = Vec(8, 8);
                addChild(left);

                auto right = createLight<rack::YellowLight>(Vec(56, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i);
                right->box.size = Vec(8, 8);
                addChild(right);
            }

            if (i >= 14)
            {
                std::cerr << 290 - i * 10 << "\n";
                auto left = createLight<rack::RedLight>(Vec(16, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i);
                left->box.size = Vec(8, 8);
                addChild(left);

                auto right = createLight<rack::RedLight>(Vec(56, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i);
                right->box.size = Vec(8, 8);
                addChild(right);
            }
        }
    }
}

