#include "RegisterCommand.hpp"
#include "MessageJson.hpp"

RegisterCommand::RegisterCommand(
    std::shared_ptr<AuthService> authService,
    std::string username,
    std::string password)
    : authService_(std::move(authService))
    , username_(std::move(username))
    , password_(std::move(password))
{
}

CommandResult RegisterCommand::execute()
{
    AuthResult result = authService_->registerUser(username_, password_);

    if (result.success)
    {
        LoginOkMsg okMsg{result.user.id, result.user.username, result.user.elo};
        return CommandResult{MessageType::LOGIN_OK, MessageJson::toJson(okMsg)};
    }

    LoginFailMsg failMsg{result.errorReason};
    return CommandResult{MessageType::LOGIN_FAIL, MessageJson::toJson(failMsg)};
}