#include <gtest/gtest.h>

#include "RegisterCommand.hpp"
#include "LoginCommand.hpp"
#include "AuthService.hpp"
#include "InMemoryUserRepository.hpp"
#include "MessageType.hpp"

class RegisterCommandTest : public ::testing::Test
{
protected:
    std::shared_ptr<InMemoryUserRepository> repo = std::make_shared<InMemoryUserRepository>();
    std::shared_ptr<AuthService> authService = std::make_shared<AuthService>(repo);
};

TEST_F(RegisterCommandTest, NewUserReturnsLoginOkWithDefaultElo)
{
    RegisterCommand cmd(authService, "dana", "secret123");
    CommandResult result = cmd.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_OK);
    EXPECT_EQ(result.payload.at("username").get<std::string>(), "dana");
    EXPECT_EQ(result.payload.at("elo").get<int>(), 1000);
}

TEST_F(RegisterCommandTest, DuplicateUsernameReturnsLoginFail)
{
    RegisterCommand first(authService, "dana", "secret123");
    first.execute();

    RegisterCommand second(authService, "dana", "otherPassword");
    CommandResult result = second.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_FAIL);
    EXPECT_EQ(result.payload.at("reason").get<std::string>(), "Username already taken");
}

TEST_F(RegisterCommandTest, EmptyUsernameReturnsLoginFail)
{
    RegisterCommand cmd(authService, "", "secret123");
    CommandResult result = cmd.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_FAIL);
}

// בדיקת אינטגרציה קטנה: המשתמש שנרשם דרך ה-Command הזה אמור להיות בר-login
// אמיתי אחר כך דרך AuthService, בדיוק כמו כל משתמש אחר.
TEST_F(RegisterCommandTest, RegisteredUserCanLoginAfterwards)
{
    RegisterCommand registerCmd(authService, "dana", "secret123");
    registerCmd.execute();

    LoginCommand loginCmd(authService, "dana", "secret123");
    CommandResult loginResult = loginCmd.execute();

    EXPECT_EQ(loginResult.type, MessageType::LOGIN_OK);
}