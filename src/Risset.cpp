#include "Risset.hpp"
#include "rack.hpp"
#include "Utils.hpp"
#include "lufu\oscillator.hpp"

namespace lufu
{
    class Voice
    {
    public:
        Voice(float base_pitch)
            : pitch_(0.25, 1.0)
            , osc_(base_pitch, 5.0)
        {}

        lufu::Oscillator pitch_;
        lufu::Oscillator osc_;
    };

    class RissetModule : public rack::Module
    {
    public:
        enum ParamIds
        {
            BASE_PITCH,
            PITCH_SPEED,
            NUM_PARAMS
        };

        enum InputIds
        {
            NUM_INPUTS
        };

        enum OutputIds
        {
            MIXED_OUTPUT,
            NUM_OUTPUTS
        };

        enum LightIds
        {
            NUM_LIGHTS
        };

    public:
        RissetModule() 
        : rack::Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
        , dt_(1.0 / rack::engineGetSampleRate())
        {
            voices_.emplace_back(204.616 / 4);
            voices_.emplace_back(204.616 / 2);
            voices_.emplace_back(204.616);
            voices_.emplace_back(2 * 204.616);
            voices_.emplace_back(4 * 204.616);

        }

        void step() override
        {
            for (auto & v : voices_)
            {
                v.osc_.tick(dt_);
            }

            float output{ 0.0f };
            for (const auto & v : voices_)
            {
                output += v.osc_.sine();
            }

            outputs[MIXED_OUTPUT].value = output;

        };

    private:
        float dt_; // step time, e.g. 1/SampleRate.
        std::vector<Voice> voices_;
    };




    RissetWidget::RissetWidget() :
        BaseModuleWidget("res/Recorder.svg")
    {
        using namespace rack;

        addChild(createParam<rack::RoundBlackKnob>(Vec(32, 98), module_, RissetModule::BASE_PITCH,  20.0f, 1000.0f, 261.626 ));
        addChild(createParam<rack::RoundBlackKnob>(Vec(32, 168), module_, RissetModule::PITCH_SPEED,-1.f, 1.0f, 0.01));
    
        addOutput(createOutput<CL1362Port>(Vec(30, 253), module_, RissetModule::MIXED_OUTPUT));
    }
    


}