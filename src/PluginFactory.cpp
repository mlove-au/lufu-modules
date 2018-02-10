#include <plugin.hpp>
#include <rack.hpp>

#include "Multiclock.hpp"
#include "DeltaWave.hpp"
#include "Repeater.hpp"
#include "Recorder.hpp"
#include "Risset.hpp"

namespace lufu
{
    rack::Plugin* plugin;
}

void init(rack::Plugin* p)
{
    const std::string SLUG("lufu");
    lufu::plugin = p;
    p->slug = SLUG;
#ifdef VERSION
    p->version = TOSTRING(VERSION);
#endif
 

    p->addModel(rack::createModel<lufu::MultiClockModuleWidget>(SLUG, "Multiclock", "Multiclock", rack::UTILITY_TAG));
    p->addModel(rack::createModel<lufu::DeltaWaveWidget>(SLUG, "Delta", "Delta", rack::UTILITY_TAG));
    p->addModel(rack::createModel<lufu::RepeaterWidget>(SLUG, "Repeater", "Repeater", rack::SAMPLER_TAG));
    p->addModel(rack::createModel<lufu::RecorderWidget>(SLUG, "Recorder", "Recorder", rack::SAMPLER_TAG));

    p->addModel(rack::createModel<lufu::RissetWidget>(SLUG, "Risset", "Risset", rack::OSCILLATOR_TAG));
}
