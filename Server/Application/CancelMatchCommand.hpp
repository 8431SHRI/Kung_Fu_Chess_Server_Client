#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "MatchmakingService.hpp"
#include "SessionRegistry.hpp"

class CancelMatchCommand : public Command
{
public:
    CancelMatchCommand(
        std::string connectionId,
        std::shared_ptr<MatchmakingService> matchmakingService,
        std::shared_ptr<SessionRegistry> sessionRegistry);

    CommandResult execute() override;

private:
    std::string connectionId_;
    std::shared_ptr<MatchmakingService> matchmakingService_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
};