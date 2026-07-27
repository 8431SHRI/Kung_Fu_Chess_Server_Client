#include <gtest/gtest.h>
#include "AuthService.hpp"
#include "InMemoryUserRepository.hpp"

// שימוש ב-InMemoryUserRepository כ"fake" אמיתי במקום mock —
// זה בדיוק התפקיד שלו: לאפשר לבדוק את AuthService בלי DB אמיתי.
class AuthServiceTest : public ::testing::Test
{
protected:
    std::shared_ptr<InMemoryUserRepository> repo =
        std::make_shared<InMemoryUserRepository>();
    AuthService authService{repo};
};

TEST_F(AuthServiceTest, RegisterNewUserSucceeds)
{
    auto result = authService.registerUser("dana", "secret123");

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.user.username, "dana");
    EXPECT_TRUE(result.errorReason.empty());
}

TEST_F(AuthServiceTest, RegisterThenLoginSucceeds)
{
    authService.registerUser("dana", "secret123");

    auto login = authService.login("dana", "secret123");

    EXPECT_TRUE(login.success);
    EXPECT_EQ(login.user.username, "dana");
}

TEST_F(AuthServiceTest, RegisterDuplicateUsernameFails)
{
    authService.registerUser("dana", "secret123");
    auto second = authService.registerUser("dana", "otherPassword");

    EXPECT_FALSE(second.success);
    EXPECT_EQ(second.errorReason, "Username already taken");
}

TEST_F(AuthServiceTest, RegisterEmptyUsernameFails)
{
    auto result = authService.registerUser("", "secret123");

    EXPECT_FALSE(result.success);
}

TEST_F(AuthServiceTest, RegisterEmptyPasswordFails)
{
    auto result = authService.registerUser("dana", "");

    EXPECT_FALSE(result.success);
}

TEST_F(AuthServiceTest, LoginWrongPasswordFails)
{
    authService.registerUser("dana", "secret123");

    auto login = authService.login("dana", "wrongPassword");

    EXPECT_FALSE(login.success);
}

TEST_F(AuthServiceTest, LoginNonexistentUserFails)
{
    auto login = authService.login("noSuchUser", "whatever");

    EXPECT_FALSE(login.success);
}

// בודק את ההתנהגות המכוונת נגד username enumeration:
// "משתמש לא קיים" ו"סיסמה שגויה" חייבים להחזיר בדיוק את אותה הודעת שגיאה.
TEST_F(AuthServiceTest, LoginMissingUserAndWrongPasswordGiveIdenticalError)
{
    authService.registerUser("dana", "secret123");

    auto loginMissingUser = authService.login("noSuchUser", "whatever");
    auto loginWrongPassword = authService.login("dana", "wrongPassword");

    EXPECT_EQ(loginMissingUser.errorReason, loginWrongPassword.errorReason);
}

TEST_F(AuthServiceTest, RegisteredUserStartsWithDefaultElo)
{
    auto result = authService.registerUser("dana", "secret123");

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.user.elo, 1000);
}