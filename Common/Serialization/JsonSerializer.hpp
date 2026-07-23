#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "MessageType.hpp"

/*
 * אחריות:
 * לעטוף כל payload בתוך מעטפת אחידה:
 *
 * {
 *      "type" : "...",
 *      "payload" : { ... }
 * }
 *
 * הקלאס הזה לא יודע כלום על GameSnapshot,
 * LoginMsg וכו'.
 */
class JsonSerializer
{
public:

    template<typename Payload>
    static std::string wrap(
        MessageType type,
        const Payload& payload)
    {
        nlohmann::json message;

        message["type"] = toString(type);
        message["payload"] = payload;

        return message.dump();
    }
};