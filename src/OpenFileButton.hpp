#pragma once

#include "../ext/osdialog/osdialog.h"
#include "rack.hpp"
#include <iostream>

using namespace rack;

class OpenFileButton : public TL1105
{
public:
    using Callback = std::function<void(const std::string &)>;

    OpenFileButton(Callback cbk, osdialog_file_action action = OSDIALOG_OPEN)
    : TL1105()
    , callback_(std::move(cbk))
    , action_(action)
    {
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
