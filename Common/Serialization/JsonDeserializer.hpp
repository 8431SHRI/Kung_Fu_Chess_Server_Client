#pragma once

#include <string>
#include <optional>

#include <nlohmann/json.hpp>

#include "MessageType.hpp"

/*
 * אחריות:
 * לפרק את מעטפת ההודעה בלבד.
 *
 * הוא לא יודע איך נראה GameSnapshot,
 * LoginMsg וכו'.
 */
class JsonDeserializer
{
public:

    static MessageType peekType(
        const std::string& raw)
    {
        try
        {
            auto json =
                nlohmann::json::parse(raw);

            return fromString(
                json.at("type").get<std::string>());
        }
        catch (...)
        {
            return MessageType::UNKNOWN;
        }
    }

    static std::optional<nlohmann::json> getPayload(
        const std::string& raw)
    {
        try
        {
            auto json =
                nlohmann::json::parse(raw);

            return json.at("payload");
        }
        catch (...)
        {
            return std::nullopt;
        }
    };
};