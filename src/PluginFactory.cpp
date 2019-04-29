#include <plugin.hpp>
#include <rack.hpp>
#include <string>
#include "Multiclock.hpp"
#include "DeltaWave.hpp"
#include "Repeater.hpp"
#include "Recorder.hpp"


namespace lufu
{
    rack::Plugin* plugin;
}

void init(rack::Plugin* p)
{
    std::string SLUG("lufu");
    lufu::plugin = p;
    p->slug = SLUG;
#ifdef VERSION
    p->version = TOSTRING(VERSION);
#endif
    p->addModel(lufu::g_multiclockModel);
    //p->addModel(rack::createModel<lufu::DeltaWaveWidget>(SLUG, std::string("Delta"), std::string("Delta"), rack::UTILITY_TAG));
   // p->addModel(rack::createModel<lufu::RepeaterWidget>(SLUG, std::string("Repeater"), std::string("Repeater"), rack::SAMPLER_TAG));
   // p->addModel(rack::createModel<lufu::RecorderWidget>(SLUG, std::string("Recorder"), std::string("Recorder"), rack::SAMPLER_TAG));
}
