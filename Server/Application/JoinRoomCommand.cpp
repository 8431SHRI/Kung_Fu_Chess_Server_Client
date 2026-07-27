// #include "JoinRoomCommand.hpp"

// #include "MessageJson.hpp"

// JoinRoomCommand::JoinRoomCommand(
//     std::string connectionId,
//     std::string roomName,
//     std::shared_ptr<AuthService> authService,
//     std::shared_ptr<RoomManager> roomManager,
//     std::shared_ptr<SessionRegistry> sessionRegistry,
//     Command::SendCallback sendCallback)
//     : connectionId_(std::move(connectionId))
//     , roomName_(std::move(roomName))
//     , authService_(std::move(authService))
//     , roomManager_(std::move(roomManager))
//     , sessionRegistry_(std::move(sessionRegistry))
//     , sendCallback_(std::move(sendCallback))
// {
// }

// CommandResult JoinRoomCommand::execute()
// {
//     auto userIdOpt = sessionRegistry_->getUserId(connectionId_);

//     if (!userIdOpt.has_value())
//     {
//         return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "must_login_before_join_room"}}};
//     }

//     // AuthService חושף רק login/registerUser - אין getter לפי userId, אז נשתמש
//     // ב-repository דרך AuthService::login-adjacent גישה עקיפה אינה קיימת בכוונה.
//     // TODO: חסר ל-AuthService (או ל-IUserRepository דרכו) getter פשוט findUserById
//     //       ציבורי-נגיש-מ-Application. כרגע JoinRoomCommand משתמש בברירת מחדל
//     //       (username/elo ריקים) עד שזה יתווסף - מסמנת כפער ידוע, לא ממציאה API.
//     std::string username = "player-" + userIdOpt.value();
//     int elo = 1000;

//     auto room = roomManager_->getOrCreate(roomName_);

//     JoinResult joinResult = room->join(connectionId_, username, elo, sendCallback_);

//     sessionRegistry_->bindGame(connectionId_, room->getName());

//     nlohmann::json payload;
//     payload["roomName"] = room->getName();
//     payload["role"] = (joinResult.role == JoinRole::PLAYER) ? "PLAYER" : "SPECTATOR";

//     return CommandResult{MessageType::ROOM_STATE, payload};
// }
#include "JoinRoomCommand.hpp"

#include "MessageJson.hpp"

JoinRoomCommand::JoinRoomCommand(
    std::string connectionId,
    std::string roomName,
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<RoomManager> roomManager,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    Command::SendCallback sendCallback)
    : connectionId_(std::move(connectionId))
    , roomName_(std::move(roomName))
    , authService_(std::move(authService))
    , roomManager_(std::move(roomManager))
    , sessionRegistry_(std::move(sessionRegistry))
    , sendCallback_(std::move(sendCallback))
{
}

CommandResult JoinRoomCommand::execute()
{
    auto userIdOpt = sessionRegistry_->getUserId(connectionId_);

    if (!userIdOpt.has_value())
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "must_login_before_join_room"}}};
    }

    auto userOpt = authService_->findUserById(userIdOpt.value());

    if (!userOpt.has_value())
    {
        // מצב לא צפוי: יש userId ב-SessionRegistry אבל אין משתמש כזה ב-repository.
        // לא אמור לקרות בזרימה תקינה, אבל לא קורסים - מחזירים שגיאה ברורה.
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "user_not_found"}}};
    }

    auto room = roomManager_->getOrCreate(roomName_);

    JoinResult joinResult = room->join(connectionId_, userOpt->username, userOpt->elo, sendCallback_);

    sessionRegistry_->bindGame(connectionId_, room->getName());

    nlohmann::json payload;
    payload["roomName"] = room->getName();
    payload["role"] = (joinResult.role == JoinRole::PLAYER) ? "PLAYER" : "SPECTATOR";

    return CommandResult{MessageType::ROOM_STATE, payload};
}