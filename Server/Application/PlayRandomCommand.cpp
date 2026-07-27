#include "PlayRandomCommand.hpp"

PlayRandomCommand::PlayRandomCommand(
    std::string connectionId,
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<MatchmakingService> matchmakingService,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    Command::SendCallback sendCallback)
    : connectionId_(std::move(connectionId))
    , authService_(std::move(authService))
    , matchmakingService_(std::move(matchmakingService))
    , sessionRegistry_(std::move(sessionRegistry))
    , sendCallback_(std::move(sendCallback))
{
}

CommandResult PlayRandomCommand::execute()
{
    auto userIdOpt = sessionRegistry_->getUserId(connectionId_);

    if (!userIdOpt.has_value())
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "must_login_before_play_random"}}};
    }

    auto userOpt = authService_->findUserById(userIdOpt.value());

    if (!userOpt.has_value())
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "user_not_found"}}};
    }

    matchmakingService_->enqueue(
        userOpt->id, userOpt->username, userOpt->elo, connectionId_, sendCallback_);

    // אין תשובה סינכרונית משמעותית - MATCH_FOUND יגיע בנפרד כש-MatchmakingService יזווג.
    return CommandResult{MessageType::PONG, nlohmann::json::object()};
}