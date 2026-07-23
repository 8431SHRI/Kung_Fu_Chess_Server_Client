#include "AuthService.hpp"
#include "PasswordHasher.hpp"

AuthService::AuthService(std::shared_ptr<IUserRepository> userRepository)
    : userRepository_(std::move(userRepository))
{
}

AuthResult AuthService::login(const std::string& username, const std::string& password)
{
    auto userOpt = userRepository_->findByUsername(username);

    // הודעת שגיאה זהה לשם משתמש לא קיים ולסיסמה שגויה בכוונה —
    // כדי לא לאפשר לתוקף לגלות אילו שמות משתמש קיימים במערכת (username enumeration)
    if (!userOpt.has_value() || !PasswordHasher::verify(password, userOpt->passwordHash))
    {
        return AuthResult{false, User{}, "Invalid username or password"};
    }

    return AuthResult{true, *userOpt, ""};
}

AuthResult AuthService::registerUser(const std::string& username, const std::string& password)
{
    if (username.empty() || password.empty())
    {
        return AuthResult{false, User{}, "Username and password must not be empty"};
    }

    if (userRepository_->existsByUsername(username))
    {
        return AuthResult{false, User{}, "Username already taken"};
    }

    std::string passwordHash = PasswordHasher::hash(password);
    User newUser = userRepository_->create(username, passwordHash);

    return AuthResult{true, newUser, ""};
}