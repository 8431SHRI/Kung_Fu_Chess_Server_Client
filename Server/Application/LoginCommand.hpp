#pragma once

#include <memory>
#include <string>

#include "Command.hpp"
#include "AuthService.hpp"

/*
 * @class LoginCommand
 * @brief פקודת login בודדת — מתווכת בין הבקשה ל-AuthService, לא מממשת אימות בעצמה.
 */
class LoginCommand : public Command
{
public:
    LoginCommand(
        std::shared_ptr<AuthService> authService,
        std::string username,
        std::string password);

    CommandResult execute() override;

private:
    std::shared_ptr<AuthService> authService_;
    std::string username_;
    std::string password_;
};