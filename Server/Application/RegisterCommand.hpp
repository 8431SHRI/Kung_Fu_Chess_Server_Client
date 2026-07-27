#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "AuthService.hpp"

/*
 * @class RegisterCommand
 * @brief פקודת register בודדת — מתווכת בין הבקשה ל-AuthService.
 * Note: הרשמה מוצלחת ממופה ל-LOGIN_OK (auto-login אחרי register), כי אין
 *       REGISTER_OK נפרד ב-MessageType הקיים. זו הנחת עיצוב - תאשרי שזה מתאים
 *       לזרימת ה-UI (LobbyWindow אחרי RegisterWindow), או שנוסיף REGISTER_OK ל-Common.
 */
class RegisterCommand : public Command
{
public:
    RegisterCommand(
        std::shared_ptr<AuthService> authService,
        std::string username,
        std::string password);

    CommandResult execute() override;

private:
    std::shared_ptr<AuthService> authService_;
    std::string username_;
    std::string password_;
};