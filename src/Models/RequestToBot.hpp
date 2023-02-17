#pragma once

#include <QString>

struct RequestToBot
{
    enum class RequestType{
        GetDescription,
        RequestToSearch,
        CheckCountVotes,
    };

    std::int64_t chat_id        = 0;
    std::string request_text    = {};
    RequestType request_type    = RequestType::RequestToSearch;

    RequestToBot(std::int64_t a_chat_id,
                 const std::string& a_request_text,
                 RequestType a_request_type)
        : chat_id(a_chat_id)
        , request_text(a_request_text)
        , request_type(a_request_type)
    {}
};
