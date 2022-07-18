#pragma once

#include <QString>

struct RequestToBot
{
    std::int64_t chat_id        = 0;
    std::string request_text    = {};
};
