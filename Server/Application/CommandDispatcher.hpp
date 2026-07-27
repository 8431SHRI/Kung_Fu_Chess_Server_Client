#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "Command.hpp"
#include "MessageType.hpp"
#include "AuthService.hpp"
#include "RoomManager.hpp"
#include "SessionRegistry.hpp"
#include "MatchmakingService.hpp"

class CommandDispatcher
{
public:
    CommandDispatcher(
        std::shared_ptr<AuthService> authService,
        std::shared_ptr<RoomManager> roomManager,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        std::shared_ptr<MatchmakingService> matchmakingService);

    CommandResult dispatch(
        const std::string& connectionId,
        MessageType type,
        const nlohmann::json& payload,
        Command::SendCallback sendCallback);

private:
    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<RoomManager> roomManager_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    std::shared_ptr<MatchmakingService> matchmakingService_;

    static CommandResult runCommand(std::unique_ptr<Command> command);
    static CommandResult errorResult(const std::string& reason);
};