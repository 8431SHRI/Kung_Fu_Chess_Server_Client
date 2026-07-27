#pragma once

#include <functional>
#include <nlohmann/json.hpp>

#include "MessageType.hpp"

struct CommandResult
{
    MessageType type;
    nlohmann::json payload;
};

class Command
{
public:
    using SendCallback = std::function<void(const std::string& rawJson)>;

    virtual ~Command() = default;

    virtual CommandResult execute() = 0;
};