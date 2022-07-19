#pragma once

#include <QString>

struct ResponseFromBot
{
    std::int64_t chat_id        = 0;
    std::string response_text   = {};

    ResponseFromBot(std::int64_t a_chat_id,
                    const std::string& a_response_text)
        : chat_id(a_chat_id)
        , response_text(a_response_text)
    {}
};
