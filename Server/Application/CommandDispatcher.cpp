#include "CommandDispatcher.hpp"

#include "LoginCommand.hpp"
#include "RegisterCommand.hpp"
#include "JoinRoomCommand.hpp"
#include "MoveCommand.hpp"
#include "PlayRandomCommand.hpp"
#include "CancelMatchCommand.hpp"
#include "MessageJson.hpp"

CommandDispatcher::CommandDispatcher(
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<RoomManager> roomManager,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    std::shared_ptr<MatchmakingService> matchmakingService)
    : authService_(std::move(authService))
    , roomManager_(std::move(roomManager))
    , sessionRegistry_(std::move(sessionRegistry))
    , matchmakingService_(std::move(matchmakingService))
{
}

CommandResult CommandDispatcher::runCommand(std::unique_ptr<Command> command)
{
    return command->execute();
}

CommandResult CommandDispatcher::errorResult(const std::string& reason)
{
    return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", reason}}};
}

CommandResult CommandDispatcher::dispatch(
    const std::string& connectionId,
    MessageType type,
    const nlohmann::json& payload,
    Command::SendCallback sendCallback)
{
    switch (type)
    {
    case MessageType::LOGIN:
    {
        auto loginMsg = MessageJson::fromLoginJson(payload);
        if (!loginMsg.has_value()) { return errorResult("invalid_login_payload"); }
        return runCommand(std::make_unique<LoginCommand>(authService_, loginMsg->username, loginMsg->password));
    }

    case MessageType::REGISTER:
    {
        auto registerMsg = MessageJson::fromRegisterJson(payload);
        if (!registerMsg.has_value()) { return errorResult("invalid_register_payload"); }
        return runCommand(std::make_unique<RegisterCommand>(authService_, registerMsg->username, registerMsg->password));
    }

    case MessageType::JOIN_ROOM:
    {
        auto joinMsg = MessageJson::fromJoinRoomJson(payload);
        if (!joinMsg.has_value()) { return errorResult("invalid_join_room_payload"); }
        return runCommand(std::make_unique<JoinRoomCommand>(
            connectionId, joinMsg->roomName, authService_, roomManager_, sessionRegistry_, sendCallback));
    }

    case MessageType::MOVE:
    {
        auto moveMsg = MessageJson::fromMoveJson(payload);
        if (!moveMsg.has_value()) { return errorResult("invalid_move_payload"); }
        return runCommand(std::make_unique<MoveCommand>(
            connectionId, moveMsg->from, moveMsg->to, sessionRegistry_, roomManager_));
    }

    case MessageType::PLAY_RANDOM:
        return runCommand(std::make_unique<PlayRandomCommand>(
            connectionId, authService_, matchmakingService_, sessionRegistry_, sendCallback));

    case MessageType::CANCEL_MATCH:
        return runCommand(std::make_unique<CancelMatchCommand>(
            connectionId, matchmakingService_, sessionRegistry_));

    default:
        return errorResult("unsupported_message_type: " + toString(type));
    }
}
void CommandDispatcher::handleDisconnect(const std::string& connectionId)
{
    auto gameIdOpt = sessionRegistry_->getGameId(connectionId);

    if (gameIdOpt.has_value())
    {
        auto room = roomManager_->find(gameIdOpt.value());

        if (room)
        {
            room->onConnectionLost(connectionId);
        }
    }

    sessionRegistry_->remove(connectionId);
}