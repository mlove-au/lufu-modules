#pragma once

#include <sstream>
#include <iomanip>
#include <vector>
#include "rack.hpp"

namespace lufu
{
    extern rack::Plugin * plugin;

    template <typename Module, size_t Width=6>
    struct BaseModuleWidget : public rack::ModuleWidget
    {
        Module * module_{ nullptr };

        template <typename... Args>
        BaseModuleWidget(const std::string & svgPanelName, Args&&... args)
        {
            using namespace rack;

            module_ = new Module(std::forward<Args>(args)...);
            setModule(module_);
            box.size = rack::Vec(Width * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

            auto panel = new rack::SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(rack::SVG::load(assetPlugin(plugin, svgPanelName)));
            addChild(panel);
            
            addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
            addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
            addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
            addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        }
    };
    
    template <typename T>
    std::string to_string_with_precision(const T a_value, const int n = 3)
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(n) << a_value;
        return out.str();
    }

    template <typename T = rack::Label>
    T * createLabel(rack::Vec pos, std::string text)
    {
        auto l = new T();
        l->box.pos = std::move(pos);
        l->text = std::move(text);
        return l;
    }

    template <typename T>
    using ref_vec = std::vector<std::reference_wrapper<T>>;

    template <typename T> ref_vec<T>
        wrap(T * data, size_t num)
        {
            return std::vector<std::reference_wrapper<T>>(data, data + num);
        }

        template <typename T>
        inline void set_all(ref_vec<T> & vec, T t)
        {
            for (auto & v : vec)
            {
                v = t;
            }
        }
}