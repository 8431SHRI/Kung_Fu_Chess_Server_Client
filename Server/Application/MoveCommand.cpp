#include "MoveCommand.hpp"

MoveCommand::MoveCommand(
    std::string connectionId,
    Position from,
    Position to,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    std::shared_ptr<RoomManager> roomManager)
    : connectionId_(std::move(connectionId))
    , from_(from)
    , to_(to)
    , sessionRegistry_(std::move(sessionRegistry))
    , roomManager_(std::move(roomManager))
{
}

CommandResult MoveCommand::execute()
{
    auto gameIdOpt = sessionRegistry_->getGameId(connectionId_);

    if (!gameIdOpt.has_value())
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "not_in_a_room"}}};
    }

    auto room = roomManager_->find(gameIdOpt.value());

    if (!room)
    {
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "room_no_longer_exists"}}};
    }

    auto playerSource = room->findPlayerSource(connectionId_);

    if (!playerSource)
    {
        // מגיע לכאן אם החיבור הוא spectator (אין לו NetworkPlayerSource בכלל) -
        // זו בדיוק החסימה של "צופה לא יכול לזוז" שדיברנו עליה ב-SpectatorSession.
        return CommandResult{MessageType::ERROR, nlohmann::json{{"reason", "spectators_cannot_move"}}};
    }

    playerSource->enqueueMove(from_, to_);

    // MOVE לא מקבל תשובה סינכרונית משמעותית - התוצאה האמיתית (הצליח/נכשל לפי
    // RuleEngine) תגיע כ-GAME_SNAPSHOT בתוך ה-tick הבא. מחזירים PONG כאישור קבלה גרידא.
    return CommandResult{MessageType::PONG, nlohmann::json::object()};
}