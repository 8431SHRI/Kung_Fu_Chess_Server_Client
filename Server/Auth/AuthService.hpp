#pragma once

#include <memory>
#include <string>

#include "../Persistence/IUserRepository.hpp"

/*
 * @struct AuthResult
 * @brief תוצאת פעולת login/register — לא Message! CommandLayer הוא זה שממיר את זה ל-LoginOkMsg/LoginFailMsg.
 */
struct AuthResult
{
    bool success = false;

    User user; // תקף רק אם success == true

    std::string errorReason; // תקף רק אם success == false
};

/*
 * @class AuthService
 * @brief login/register מול IUserRepository בלבד.
 * Purpose: לוגיקה עסקית של אימות משתמשים; לא נוגע ב-DB ישירות ולא בסיסמה גולמית מעבר לאימות.
 * Used by: LoginCommand, RegisterCommand.
 */
class AuthService
{
public:
    explicit AuthService(std::shared_ptr<IUserRepository> userRepository);

    AuthResult login(const std::string& username, const std::string& password);

    AuthResult registerUser(const std::string& username, const std::string& password);

private:
    std::shared_ptr<IUserRepository> userRepository_;
};