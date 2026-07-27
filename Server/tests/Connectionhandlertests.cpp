#include <gtest/gtest.h>
#include <vector>

#include <nlohmann/json.hpp>

#include "ConnectionHandler.hpp"
#include "CommandDispatcher.hpp"
#include "SessionRegistry.hpp"
#include "AuthService.hpp"
#include "InMemoryUserRepository.hpp"
#include "MessageType.hpp"

class ConnectionHandlerTest : public ::testing::Test
{
protected:
    std::shared_ptr<InMemoryUserRepository> repo = std::make_shared<InMemoryUserRepository>();
    std::shared_ptr<AuthService> authService = std::make_shared<AuthService>(repo);
    std::shared_ptr<CommandDispatcher> dispatcher = std::make_shared<CommandDispatcher>(authService);
    std::shared_ptr<SessionRegistry> sessionRegistry = std::make_shared<SessionRegistry>();

    std::vector<std::string> sentMessages;

    // SendCallback שרק אוסף מחרוזות - בדיוק כמו שהוצע: בלי socket אמיתי בכלל.
    std::unique_ptr<ConnectionHandler> makeHandler(const std::string &connectionId = "conn-1")
    {
        return std::make_unique<ConnectionHandler>(
            connectionId,
            dispatcher,
            sessionRegistry,
            [this](const std::string &outgoing)
            { sentMessages.push_back(outgoing); });
    }

    static std::string buildRawMessage(const std::string &type, const nlohmann::json &payload)
    {
        nlohmann::json msg = {{"type", type}, {"payload", payload}};
        return msg.dump();
    }
};

TEST_F(ConnectionHandlerTest, GetConnectionIdReturnsIdPassedToConstructor)
{
    auto handler = makeHandler("conn-42");

    EXPECT_EQ(handler->getConnectionId(), "conn-42");
}

TEST_F(ConnectionHandlerTest, ValidLoginSendsLoginOkAndBindsSessionRegistry)
{
    authService->registerUser("dana", "secret123");
    auto handler = makeHandler("conn-1");

    handler->handleMessage(buildRawMessage("LOGIN", {{"username", "dana"}, {"password", "secret123"}}));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    EXPECT_EQ(response.at("type").get<std::string>(), "LOGIN_OK");
    EXPECT_EQ(response.at("payload").at("username").get<std::string>(), "dana");

    auto boundUserId = sessionRegistry->getUserId("conn-1");
    ASSERT_TRUE(boundUserId.has_value());
    EXPECT_EQ(*boundUserId, response.at("payload").at("userId").get<std::string>());
}

TEST_F(ConnectionHandlerTest, WrongPasswordSendsLoginFailAndDoesNotBindSession)
{
    authService->registerUser("dana", "secret123");
    auto handler = makeHandler("conn-1");

    handler->handleMessage(buildRawMessage("LOGIN", {{"username", "dana"}, {"password", "wrongPassword"}}));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    EXPECT_EQ(response.at("type").get<std::string>(), "LOGIN_FAIL");

    EXPECT_FALSE(sessionRegistry->getUserId("conn-1").has_value());
}

TEST_F(ConnectionHandlerTest, ValidRegisterAlsoBindsSessionRegistry)
{
    // RegisterCommand ממופה ל-LOGIN_OK (auto-login) - ConnectionHandler צריך
    // לזהות את זה ולרשום session בדיוק כמו ב-login רגיל.
    auto handler = makeHandler("conn-1");

    handler->handleMessage(buildRawMessage("REGISTER", {{"username", "newUser"}, {"password", "pw123456"}}));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    EXPECT_EQ(response.at("type").get<std::string>(), "LOGIN_OK");

    EXPECT_TRUE(sessionRegistry->getUserId("conn-1").has_value());
}

TEST_F(ConnectionHandlerTest, CompletelyInvalidJsonDoesNotCrashAndSendsError)
{
    auto handler = makeHandler("conn-1");

    EXPECT_NO_THROW(handler->handleMessage("this is not json at all {{{"));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    EXPECT_EQ(response.at("type").get<std::string>(), "ERROR");
}

TEST_F(ConnectionHandlerTest, ValidJsonWithUnknownTypeSendsError)
{
    auto handler = makeHandler("conn-1");

    handler->handleMessage(buildRawMessage("NOT_A_REAL_TYPE", nlohmann::json::object()));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    EXPECT_EQ(response.at("type").get<std::string>(), "ERROR");
}

TEST_F(ConnectionHandlerTest, MissingPayloadFieldTreatedAsEmptyPayloadNotCrash)
{
    auto handler = makeHandler("conn-1");

    nlohmann::json msgWithoutPayload = {{"type", "LOGIN"}}; // בלי מפתח "payload" בכלל

    EXPECT_NO_THROW(handler->handleMessage(msgWithoutPayload.dump()));

    ASSERT_EQ(sentMessages.size(), 1u);

    nlohmann::json response = nlohmann::json::parse(sentMessages[0]);
    // payload ריק -> MessageJson::fromLoginJson נכשל -> CommandDispatcher מחזיר ERROR
    EXPECT_EQ(response.at("type").get<std::string>(), "ERROR");
}

TEST_F(ConnectionHandlerTest, OnDisconnectRemovesSessionFromRegistry)
{
    authService->registerUser("dana", "secret123");
    auto handler = makeHandler("conn-1");

    handler->handleMessage(buildRawMessage("LOGIN", {{"username", "dana"}, {"password", "secret123"}}));
    ASSERT_TRUE(sessionRegistry->getUserId("conn-1").has_value());

    handler->onDisconnect();

    EXPECT_FALSE(sessionRegistry->getUserId("conn-1").has_value());
}

TEST_F(ConnectionHandlerTest, OnDisconnectBeforeAnyLoginDoesNotCrash)
{
    auto handler = makeHandler("conn-1");

    EXPECT_NO_THROW(handler->onDisconnect());
}

TEST_F(ConnectionHandlerTest, TwoConnectionsAreTrackedIndependently)
{
    authService->registerUser("dana", "secret123");
    authService->registerUser("noam", "otherPass456");

    auto handlerA = makeHandler("conn-A");
    auto handlerB = makeHandler("conn-B");

    handlerA->handleMessage(buildRawMessage("LOGIN", {{"username", "dana"}, {"password", "secret123"}}));
    handlerB->handleMessage(buildRawMessage("LOGIN", {{"username", "noam"}, {"password", "otherPass456"}}));

    auto userA = sessionRegistry->getUserId("conn-A");
    auto userB = sessionRegistry->getUserId("conn-B");

    ASSERT_TRUE(userA.has_value());
    ASSERT_TRUE(userB.has_value());
    EXPECT_NE(*userA, *userB);

    handlerA->onDisconnect();

    EXPECT_FALSE(sessionRegistry->getUserId("conn-A").has_value());
    EXPECT_TRUE(sessionRegistry->getUserId("conn-B").has_value()); // B לא הושפע מ-disconnect של A
}