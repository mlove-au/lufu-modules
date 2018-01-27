#pragma once

#include "../ext/osdialog/osdialog.h"
#include "rack.hpp"
#include <iostream>

using namespace rack;

class OpenFileButton : public TL1105
{
public:
    using Callback = std::function<void(const std::string &)>;

    OpenFileButton(Callback cbk)
    : TL1105()
    , callback_(std::move(cbk))
    {
    }

    
    void onChange(EventChange& e) override
    {
        if (value == 1.0)
        {
            std::string lastPath;
            std::string dir = lastPath.empty() ? assetLocal("") : extractDirectory("C://");

            char* path = osdialog_file(OSDIALOG_OPEN, dir.c_str(), NULL, NULL);
            if (path && callback_)
            {
                callback_(std::string(path));
            }
            free(path);
        }
    }


private:
    Callback callback_;

};
