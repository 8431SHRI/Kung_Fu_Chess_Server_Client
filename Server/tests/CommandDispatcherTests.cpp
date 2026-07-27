#include <gtest/gtest.h>

#include "CommandDispatcher.hpp"
#include "AuthService.hpp"
#include "InMemoryUserRepository.hpp"
#include "MessageType.hpp"

class CommandDispatcherTest : public ::testing::Test
{
protected:
    std::shared_ptr<InMemoryUserRepository> repo = std::make_shared<InMemoryUserRepository>();
    std::shared_ptr<AuthService> authService = std::make_shared<AuthService>(repo);
    CommandDispatcher dispatcher{authService};
};

// ---------- LOGIN ----------

TEST_F(CommandDispatcherTest, DispatchLoginWithValidPayloadSucceeds)
{
    authService->registerUser("dana", "secret123");

    nlohmann::json payload = {{"username", "dana"}, {"password", "secret123"}};
    CommandResult result = dispatcher.dispatch(MessageType::LOGIN, payload);

    EXPECT_EQ(result.type, MessageType::LOGIN_OK);
    EXPECT_EQ(result.payload.at("username").get<std::string>(), "dana");
}

TEST_F(CommandDispatcherTest, DispatchLoginWithWrongPasswordReturnsLoginFailNotError)
{
    // חשוב להבדיל: כישלון עסקי (סיסמה שגויה) הוא LOGIN_FAIL, לא ERROR -
    // ERROR שמור לבעיות ברמת ה-payload/protocol עצמו.
    authService->registerUser("dana", "secret123");

    nlohmann::json payload = {{"username", "dana"}, {"password", "wrongPassword"}};
    CommandResult result = dispatcher.dispatch(MessageType::LOGIN, payload);

    EXPECT_EQ(result.type, MessageType::LOGIN_FAIL);
}

TEST_F(CommandDispatcherTest, DispatchLoginWithMissingPasswordFieldReturnsError)
{
    nlohmann::json payload = {{"username", "dana"}}; // חסר password

    CommandResult result = dispatcher.dispatch(MessageType::LOGIN, payload);

    EXPECT_EQ(result.type, MessageType::ERROR);
    EXPECT_EQ(result.payload.at("reason").get<std::string>(), "invalid_login_payload");
}

TEST_F(CommandDispatcherTest, DispatchLoginWithNonObjectPayloadReturnsErrorNotCrash)
{
    nlohmann::json payload = "not an object";

    CommandResult result = dispatcher.dispatch(MessageType::LOGIN, payload);

    EXPECT_EQ(result.type, MessageType::ERROR);
}

// ---------- REGISTER ----------

TEST_F(CommandDispatcherTest, DispatchRegisterWithValidPayloadSucceeds)
{
    nlohmann::json payload = {{"username", "newUser"}, {"password", "pw123456"}};

    CommandResult result = dispatcher.dispatch(MessageType::REGISTER, payload);

    EXPECT_EQ(result.type, MessageType::LOGIN_OK);
}

TEST_F(CommandDispatcherTest, DispatchRegisterWithMissingUsernameFieldReturnsError)
{
    nlohmann::json payload = {{"password", "pw123456"}}; // חסר username

    CommandResult result = dispatcher.dispatch(MessageType::REGISTER, payload);

    EXPECT_EQ(result.type, MessageType::ERROR);
    EXPECT_EQ(result.payload.at("reason").get<std::string>(), "invalid_register_payload");
}

// ---------- Unsupported message types (פער ידוע - ראו TODO ב-CommandDispatcher.cpp) ----------

TEST_F(CommandDispatcherTest, DispatchMoveReturnsUnsupportedError_KnownGap)
{
    // MOVE ממתין ל-Server/Network (SessionRegistry) כדי לדעת לאיזה GameSession
    // לנתב - עדיין לא ממומש. הטסט הזה מתעד את הפער במפורש, לא בודק "תקלה".
    CommandResult result = dispatcher.dispatch(MessageType::MOVE, nlohmann::json{});

    EXPECT_EQ(result.type, MessageType::ERROR);
    EXPECT_NE(
        result.payload.at("reason").get<std::string>().find("unsupported_message_type"),
        std::string::npos);
}

TEST_F(CommandDispatcherTest, DispatchJoinRoomReturnsUnsupportedError_KnownGap)
{
    // JOIN_ROOM ממתין ל-Server/Rooms + Server/Matchmaking - עדיין לא ממומש.
    CommandResult result = dispatcher.dispatch(MessageType::JOIN_ROOM, nlohmann::json{});

    EXPECT_EQ(result.type, MessageType::ERROR);
}

TEST_F(CommandDispatcherTest, DispatchPingReturnsUnsupportedError_KnownGap)
{
    CommandResult result = dispatcher.dispatch(MessageType::PING, nlohmann::json{});

    EXPECT_EQ(result.type, MessageType::ERROR);
}