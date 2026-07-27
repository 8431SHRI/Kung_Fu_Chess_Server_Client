#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "AuthService.hpp"
#include "RoomManager.hpp"
#include "SessionRegistry.hpp"

/*
 * @class JoinRoomCommand
 * @brief מצרפת חיבור לחדר לפי שם - יוצרת את החדר אם לא קיים.
 * Purpose: מתווכת בין הבקשה ל-RoomManager/Room; לא מממשת לוגיקת חדר בעצמה.
 * Note: דורשת שהחיבור כבר עבר LOGIN (יש לו userId ב-SessionRegistry) - אחרת מחזירה שגיאה.
 */
class JoinRoomCommand : public Command
{
public:
    JoinRoomCommand(
        std::string connectionId,
        std::string roomName,
        std::shared_ptr<AuthService> authService,
        std::shared_ptr<RoomManager> roomManager,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        Command::SendCallback sendCallback);

    CommandResult execute() override;

private:
    std::string connectionId_;
    std::string roomName_;
    std::shared_ptr<AuthService> authService_;
    std::shared_ptr<RoomManager> roomManager_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    Command::SendCallback sendCallback_;
};