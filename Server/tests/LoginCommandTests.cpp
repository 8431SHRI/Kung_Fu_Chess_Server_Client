#include <gtest/gtest.h>

#include "LoginCommand.hpp"
#include "AuthService.hpp"
#include "InMemoryUserRepository.hpp"
#include "MessageType.hpp"

class LoginCommandTest : public ::testing::Test
{
protected:
    std::shared_ptr<InMemoryUserRepository> repo = std::make_shared<InMemoryUserRepository>();
    std::shared_ptr<AuthService> authService = std::make_shared<AuthService>(repo);
};

TEST_F(LoginCommandTest, SuccessfulLoginReturnsLoginOkWithUserData)
{
    authService->registerUser("dana", "secret123");

    LoginCommand cmd(authService, "dana", "secret123");
    CommandResult result = cmd.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_OK);
    EXPECT_EQ(result.payload.at("username").get<std::string>(), "dana");
    EXPECT_EQ(result.payload.at("elo").get<int>(), 1000);
    EXPECT_FALSE(result.payload.at("userId").get<std::string>().empty());
}

TEST_F(LoginCommandTest, WrongPasswordReturnsLoginFailWithNonEmptyReason)
{
    authService->registerUser("dana", "secret123");

    LoginCommand cmd(authService, "dana", "wrongPassword");
    CommandResult result = cmd.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_FAIL);
    EXPECT_FALSE(result.payload.at("reason").get<std::string>().empty());
}

TEST_F(LoginCommandTest, NonexistentUserReturnsLoginFail)
{
    LoginCommand cmd(authService, "noSuchUser", "whatever");
    CommandResult result = cmd.execute();

    EXPECT_EQ(result.type, MessageType::LOGIN_FAIL);
}

// אותה בדיקה שכבר יש ב-AuthServiceTests, אבל כאן ברמת ה-Command - מוודאים
// שההגנה מפני username enumeration לא "נשברת" תוך כדי המרה ל-CommandResult/JSON.
TEST_F(LoginCommandTest, MissingUserAndWrongPasswordGiveIdenticalReason)
{
    authService->registerUser("dana", "secret123");

    LoginCommand missingUserCmd(authService, "noSuchUser", "whatever");
    LoginCommand wrongPasswordCmd(authService, "dana", "wrongPassword");

    CommandResult missingUserResult = missingUserCmd.execute();
    CommandResult wrongPasswordResult = wrongPasswordCmd.execute();

    EXPECT_EQ(
        missingUserResult.payload.at("reason").get<std::string>(),
        wrongPasswordResult.payload.at("reason").get<std::string>());
}