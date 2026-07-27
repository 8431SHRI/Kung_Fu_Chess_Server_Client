#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "RoomManager.hpp"
#include "SessionRegistry.hpp"

/*
 * @class MoveCommand
 * @brief מנתבת MOVE מחיבור נתון ל-NetworkPlayerSource הנכון בתוך ה-GameSession שלו.
 * Purpose: לא נוגעת ב-RuleEngine/GameEngine ישירות - רק דוחפת ל-NetworkPlayerSource,
 *          שממנו GameSession::tick() כבר שולף (ראו pollPlayerSource).
 * Note: דורשת שהחיבור נמצא בחדר (bindGame כבר קרה ב-JoinRoomCommand).
 */
class MoveCommand : public Command
{
public:
    MoveCommand(
        std::string connectionId,
        Position from,
        Position to,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        std::shared_ptr<RoomManager> roomManager);

    CommandResult execute() override;

private:
    std::string connectionId_;
    Position from_;
    Position to_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    std::shared_ptr<RoomManager> roomManager_;
};