
#include "Repeater.hpp"
#include "osdialog.h"
#include "DiscreteKnob.hpp"
#include "LabelledKnob.hpp"
#include "OpenFileButton.hpp"
#include "Utils.hpp"
#include "ReallySmallBlackKnob.hpp"
#include "rack.hpp"
#include <iomanip>
#include <mutex>
#include <stdio.h>
#include <thread>

#include "AudioFile/AudioFile.h"
#include "lufu/continuous_iterator.hpp"

using namespace rack;

namespace lufu
{

    struct Repeater : rack::Module
    {
        enum ParamIds
        {
            ON_OFF_PARAM,
            SPEED_PARAM,
            SPEED_CV_DEPTH,
            NUM_PARAMS
        };

        enum InputIds
        {
            RESTART_TRIGGER,
            SPEED_CV_AMOUNT,
            NUM_INPUTS
        };

        enum OutputIds
        {
            AUDIO_OUTPUT_L,
            AUDIO_OUTPUT_R,
            NUM_OUTPUTS
        };

        enum LightIds
        {
            NUM_LIGHTS
        };

        Repeater()
            : rack::Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
        {
        }

        json_t* toJson() override
        {
            json_t* rootJ = json_object();
            json_object_set_new(rootJ, "file", json_string(filename_.c_str()));
            return rootJ;
        }

        void fromJson(json_t* rootJ) override
        {
            json_t* file = json_object_get(rootJ, "file");
            if (file)
            {
                load_sample(json_string_value(file));
            }
        }

        std::vector<std::vector<float>> samples_;
        std::mutex mutex_;
        bool running = false;
        continuous_iterator<std::vector<float>> left_;
        continuous_iterator<std::vector<float>> right_;
        std::string filename_;


        void step() override
        {
            if (params[ON_OFF_PARAM].value == 0.0)
            {
                outputs[AUDIO_OUTPUT_L].value = 0.0;
                outputs[AUDIO_OUTPUT_R].value = 0.0;
                return;
            }

            if (inputs[RESTART_TRIGGER].value >= 1.0)
            {
                left_.idx_ = 0.0;
                right_.idx_ = 0.0;
            }

            std::lock_guard<std::mutex> lock(mutex_);

            auto cv_mod = 1.0;
            if (inputs[SPEED_CV_AMOUNT].active)
            {
                cv_mod = rescalef(params[SPEED_CV_DEPTH].value * inputs[SPEED_CV_AMOUNT].value, 0.0, 10.0, -1.0, 1.0);
            }

            if (left_)
            {
                left_ += params[SPEED_PARAM].value * cv_mod;
                outputs[AUDIO_OUTPUT_L].value = *left_;
            }

            if (right_)
            {
                right_ += params[SPEED_PARAM].value * cv_mod;
                outputs[AUDIO_OUTPUT_R].value = *right_;
            }
        }

        void load_sample(const std::string& path)
        {
            AudioFile<float> tmp;
            if (!tmp.load(path))
            {
                return;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            samples_.clear();
            samples_ = std::move(tmp.samples);
            left_.reset();
            right_.reset();
            if (samples_.size() >= 1)
            {
                left_ = continuous_iterator<std::vector<float>>(samples_[0]);
                if (samples_.size() >= 2)
                {
                    right_ = continuous_iterator<std::vector<float>>(samples_[1]);
                }
            }
            
            filename_ = path;
        }
    };

   

    RepeaterWidget::RepeaterWidget(Repeater * module)
        : BaseModuleWidget(module, "res/Repeater.svg")
    {

        addChild(createParam<rack::NKK>(Vec(32, 48), module, Repeater::ON_OFF_PARAM, 0.0, 1.0, 1.0));

        auto open_file = new OpenFileButton(
            [this](const std::string& path)
        {
            this->getModule()->load_sample(path);
        });
        open_file->box.pos = Vec(40, 98);
        addChild(open_file);
        
        using SpeedKnob = LabelledKnob<rack::RoundBlackKnob>;

        auto playback_speed = dynamic_cast<SpeedKnob*>(createParam<SpeedKnob>(Vec(28, 140), module, Repeater::SPEED_PARAM, -2.0, 2.0, 1.0));
        addParam(playback_speed);

        auto l = createLabel<Label>(Vec(3, 180), "Speed");
        addChild(l);
        playback_speed->setLabel(l, [](float v) { return std::string("Speed " + to_string_with_precision(v, 3)); });


        addInput(createInput<PJ301MPort>(Vec(15, 203), module, Repeater::SPEED_CV_AMOUNT));
        addParam(createParam<ReallySmallBlackKnob>(Vec(52, 205), module, Repeater::SPEED_CV_DEPTH, 0.0, 1.0, 1.0));
        addChild(createLabel(Vec(7, 228), "Speed Mod"));

        addInput(createInput<PJ301MPort>(Vec(33, 260), module, Repeater::RESTART_TRIGGER));
        addChild(createLabel(Vec(24, 285), "Sync"));

        addOutput(createOutput<CL1362Port>(Vec(10, 310), module, Repeater::AUDIO_OUTPUT_L));


        addOutput(createOutput<CL1362Port>(Vec(50, 310), module, Repeater::AUDIO_OUTPUT_R));

    }
}