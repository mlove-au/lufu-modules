#pragma once

#include "../ext/osdialog/osdialog.h"
#include "rack.hpp"
#include <functional>
#include <iostream>

using namespace rack;

namespace lufu
{
    class OpenFileButton : public rack::SVGSwitch, rack::MomentarySwitch
    {
    public:
        using Callback = std::function<void(const std::string &)>;

        OpenFileButton(Callback cbk, osdialog_file_action action = OSDIALOG_OPEN)
            : callback_(std::move(cbk))
            , action_(action)
        {
            extern rack::Plugin * plugin;
            addFrame(rack::SVG::load(assetPlugin(plugin, "res/FolderIcon.svg")));
            box.size = Vec(24, 24);
        }


        void onChange(EventChange& e) override
        {
            if (value == 1.0)
            {
                std::string lastPath;
                std::string dir = lastPath.empty() ? assetLocal("") : extractDirectory("C://");

                char* path = osdialog_file(action_, dir.c_str(), NULL, NULL);
                if (path && callback_)
                {
                    callback_(std::string(path));
                }
                free(path);
            }
        }


    private:
        Callback callback_;
        osdialog_file_action action_;
    };

}
