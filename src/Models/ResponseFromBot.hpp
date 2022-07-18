#pragma once

#include <QString>

struct ResponseFromBot
{
    std::int64_t chat_id        = 0;
    std::string response_text   = {};
};
