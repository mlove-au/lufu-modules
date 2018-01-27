#pragma once

#include "rack.hpp"
#include <functional>
#include <string>

template <typename BaseKnob>
class LabelledKnob : public BaseKnob
{
public:
    static_assert(std::is_base_of<rack::Knob, BaseKnob>::value,
                  "LabelledKnob can only be applied to types derived from "
                  "'rack::Knob', sorry.");

    using LabelFunc = std::function<std::string(float)>;

    LabelledKnob()
    : BaseKnob()
    , label_(nullptr)
    , format_(nullptr)
    {
    }

    void setLabel(rack::Label* label, LabelFunc f)
    {
        label_ = label;
        format_ = f;
        label_->text = format_(BaseKnob::value);
    }

    void onChange(rack::EventChange& e) override
    {
        BaseKnob::onChange(e);

        if (label_ && format_)
        {
            label_->text = format_(BaseKnob::value);
        }
    }

private:
    rack::Label * label_;
    LabelFunc format_;
};
