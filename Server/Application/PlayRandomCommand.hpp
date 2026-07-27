#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "AuthService.hpp"
#include "MatchmakingService.hpp"
#include "SessionRegistry.hpp"

class PlayRandomCommand : public Command
{
public:
    PlayRandomCommand(
        std::string connectionId,
        std::shared_ptr<AuthService> authService,
        std::shared_ptr<MatchmakingService> matchmakingService,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        Command::SendCallback sendCallback);

    CommandResult execute() override;

private:
    std::string connectionId_;
    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<MatchmakingService> matchmakingService_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    Command::SendCallback sendCallback_;
};