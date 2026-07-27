#include "CancelMatchCommand.hpp"

CancelMatchCommand::CancelMatchCommand(
    std::string connectionId,
    std::shared_ptr<MatchmakingService> matchmakingService,
    std::shared_ptr<SessionRegistry> sessionRegistry)
    : connectionId_(std::move(connectionId))
    , matchmakingService_(std::move(matchmakingService))
    , sessionRegistry_(std::move(sessionRegistry))
{
}

CommandResult CancelMatchCommand::execute()
{
    auto userIdOpt = sessionRegistry_->getUserId(connectionId_);

    if (!userIdOpt.has_value())
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "must_login_before_cancel_match"}}};
    }

    matchmakingService_->cancel(userIdOpt.value());

    return CommandResult{MessageType::PONG, nlohmann::json::object()};
}