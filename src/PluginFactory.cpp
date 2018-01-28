#include <plugin.hpp>
#include <rack.hpp>

#include "MultiClock.hpp"
#include "DeltaWave.hpp"
#include "Repeater.hpp"

rack::Plugin* plugin;

void init(rack::Plugin* p)
{
    const std::string SLUG("lufu");
    plugin = p;
    p->slug = SLUG;
#ifdef VERSION
    p->version = TOSTRING(VERSION);
#endif
 

    p->addModel(rack::createModel<MulticlockModuleWidget>(SLUG, "Multiclock", "Multiclock", rack::UTILITY_TAG));
    p->addModel(rack::createModel<DeltaWaveWidget>(SLUG, "Delta", "Delta", rack::UTILITY_TAG));
    p->addModel(rack::createModel<RepeaterWidget>(SLUG, "Repeater", "Repeater", rack::SAMPLER_TAG));
}
