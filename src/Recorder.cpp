#include "rack.hpp"
#include "Recorder.hpp"
#include "WavWriter.hpp"
#include "OpenFileButton.hpp"
#include "Utils.hpp"
#include <iostream>
#include "dsp/vumeter.hpp"
#include <stdio.h>
#include <chrono>
#include "componentLibrary.hpp"
#include <functional>

namespace lufu
{
    template <typename Light>
    struct VUSegment : public Light
    {
        VUSegment(int radius = 8) : Light()
        {
            this->box.size = rack::Vec(radius, radius);
        }
    };

    struct RecordingStateLight : public rack::ModuleLightWidget
    {
        RecordingStateLight()
        {
            this->box.size = rack::Vec(15, 15);
            addBaseColor(nvgRGBAf(1.0, 1.0, 0.0, 1.0)); // yellow
            addBaseColor(nvgRGBAf(0.0, 1.0, 0.0, 1.0)); // green  
            addBaseColor(nvgRGBAf(1.0, 0.0, 0.0, 1.0)); // red
        }
    };

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
            RECORDING_NO_FILE,
            RECORDING_READY,
            RECORDING_RUNNING,
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
            meter_left_.dBInterval = 3;
            meter_right_.dBInterval = 3;
            sample_rate_ = rack::engineGetSampleRate();

            lights[RECORDING_NO_FILE].setBrightness(1.0);
            lights[RECORDING_READY].setBrightness(1.0);

        }

        void set_recording_time_label(rack::Label * label)
        {
            recording_time_label_ = label;
        }

        void onSampleRateChange() override
        {
            if (sample_rate_ != rack::engineGetSampleRate())
            {
                std::cerr << "Whoa! Sample rate changed during recording!\n";
            }
        }

        void on_set_target_file(std::string path)
        {
            target_file_ = std::move(path);
            std::cout << "Saving to filename " << target_file_ << "\n";
        }

        void update_vu_lights(float left, float right)
        {
            meter_left_.setValue(left / 5.0);
            meter_right_.setValue(right / 5.0);

            for (int l = 0; l < VU_METER_LIGHTS; l++)
            {
                lights[l + VU_METER_LEFT_1].setBrightnessSmooth(meter_left_.getBrightness(VU_METER_LIGHTS - l));
                lights[l + VU_METER_RIGHT_1].setBrightnessSmooth(meter_right_.getBrightness(VU_METER_LIGHTS - l));
            }
        }

        bool is_recording() const
        {
            return sink_ != nullptr;
        }


        void update_recording_state()
        {
            lights[RECORDING_RUNNING].value = 0.0;
            lights[RECORDING_NO_FILE].value = 0.0;
            lights[RECORDING_READY].value = 0.0;
            
            if (is_recording())
            {
                const auto brightness = ticks_ % sample_rate_ > sample_rate_ / 2 ? 0.25 : 1.0;
                lights[RECORDING_RUNNING].value = 1.0;
                lights[RECORDING_RUNNING].setBrightness(brightness);
            }
            else if (target_file_.empty())
            {
                lights[RECORDING_NO_FILE].value = 1.0;
            }
            else
            {
                lights[RECORDING_READY].value = 1.0;
            }
        }


        void update_recording_time()
        {
            if (!recording_time_label_)
            {
                return;
            }

            if (ticks_ % sample_rate_ == 0)
            {
                if (!is_recording())
                {
                    recording_time_label_->text = "00:00:00";
                }
                else
                {
                    char time_str_[9];
                    const int sec = recording_seconds_ % 60;
                    const int min = (recording_seconds_ / 60) % 60;
                    const int hour = (recording_seconds_ / (60 * 60)) % 24;
                    sprintf(time_str_, "%02d:%02d:%02d", hour, min, sec);
                    recording_time_label_->text.assign(time_str_);
                    recording_seconds_++;
                }
            }
        }

        void step() override
        {
            float left = inputs[INPUT_L].active ? inputs[INPUT_L].value : 0.0;
            float right = inputs[INPUT_R].active ? inputs[INPUT_R].value : 0.0;

            update_vu_lights(left, right);
            update_recording_time();
            update_recording_state();

            ticks_++;

            // Switched off.
            if (target_file_.empty() || !params[RECORD_STOP_BUTTON].value)
            {
                if (sink_)
                {
                    sink_.reset();
                }
                return;
            }

            if (!sink_)
            {
                sink_ = std::unique_ptr<WavSink>(new WavSink(target_file_, sample_rate_));
                recording_seconds_ = 0;
            }
            sink_->push_samples(left, right);
        }

    private:
        rack::Label * recording_time_label_{ nullptr };
        uint64_t ticks_{ 0 };
        uint64_t recording_seconds_{ 0 };
        std::string target_file_;
        std::unique_ptr<lufu::WavSink> sink_;
        rack::VUMeter meter_left_;
        rack::VUMeter meter_right_;
        int32_t sample_rate_;
    };


    // Adjust position so that is centered horizontally in outer
    void center_horiz(const rack::Widget& other, rack::Widget & that)
    {
        auto center = other.box.size.x / 2;
        auto width = that.box.size.x / 2;

        that.box.pos.x = center - width;
    }

    RecorderWidget::RecorderWidget(RecorderModule * module)
        : BaseModuleWidget(module, "res/Recorder.svg")
    {
        auto rec = createParam<rack::NKK>(Vec(32, 48), module, RecorderModule::RECORD_STOP_BUTTON, 0.0, 1.0, 1.0);
        center_horiz(*this, *rec);
        addParam(rec);

        auto open_file = new OpenFileButton([this](const std::string& path)
        {
            this->getModule()->on_set_target_file(path);
        }, OSDIALOG_SAVE);

        open_file->box.pos = Vec(25, 96);
        addChild(open_file);

        addChild(createLight<RecordingStateLight>(Vec(56, 96), module, RecorderModule::RECORDING_NO_FILE));

        auto time_label = createLabel(Vec(15, 115), "00:00:00");
        addChild(time_label);
        getModule()->set_recording_time_label(time_label);

        addInput(createInput<PJ301MPort>(Vec(17, 310), module, RecorderModule::INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(52, 310), module, RecorderModule::INPUT_R));

        using Green = VUSegment<rack::GreenLight>;
        using Yellow = VUSegment<rack::YellowLight>;
        using Red = VUSegment<rack::RedLight>;

        for (int i = 0; i < RecorderModule::VU_METER_LIGHTS; i++)
        {
            const int leftX = 25;
            const int rightX = 60;

            if (i < 10)
            {
                addChild(createLight<Green>(Vec(leftX, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i));
                addChild(createLight<Green>(Vec(rightX, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i));
            }

            if (i >= 10 && i < 14)
            {
                addChild(createLight<Yellow>(Vec(leftX, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i));
                addChild(createLight<Yellow>(Vec(rightX, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i));
            }

            if (i >= 14)
            {
                addChild(createLight<Red>(Vec(leftX, 290 - (i * 10)), module, RecorderModule::VU_METER_LEFT_1 + i));
                addChild(createLight<Red>(Vec(rightX, 290 - (i * 10)), module, RecorderModule::VU_METER_RIGHT_1 + i));
            }

        }
    }
}
