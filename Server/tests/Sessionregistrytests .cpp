#include <gtest/gtest.h>

#include "SessionRegistry.hpp"

TEST(SessionRegistryTest, GetUserIdReturnsNulloptForUnknownConnection)
{
    SessionRegistry registry;

    EXPECT_FALSE(registry.getUserId("conn-1").has_value());
}

TEST(SessionRegistryTest, GetGameIdReturnsNulloptForUnknownConnection)
{
    SessionRegistry registry;

    EXPECT_FALSE(registry.getGameId("conn-1").has_value());
}

TEST(SessionRegistryTest, BindUserThenGetUserIdReturnsIt)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");

    auto userId = registry.getUserId("conn-1");

    ASSERT_TRUE(userId.has_value());
    EXPECT_EQ(*userId, "user-42");
}

TEST(SessionRegistryTest, BindGameThenGetGameIdReturnsIt)
{
    SessionRegistry registry;
    registry.bindGame("conn-1", "game-7");

    auto gameId = registry.getGameId("conn-1");

    ASSERT_TRUE(gameId.has_value());
    EXPECT_EQ(*gameId, "game-7");
}

TEST(SessionRegistryTest, UserAndGameAreIndependentPerConnection)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");
    registry.bindGame("conn-1", "game-7");

    auto userId = registry.getUserId("conn-1");
    auto gameId = registry.getGameId("conn-1");

    ASSERT_TRUE(userId.has_value());
    ASSERT_TRUE(gameId.has_value());
    EXPECT_EQ(*userId, "user-42");
    EXPECT_EQ(*gameId, "game-7");
}

TEST(SessionRegistryTest, RebindUserOverwritesPreviousValue_LastWriteWins)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");
    registry.bindUser("conn-1", "user-99");

    auto userId = registry.getUserId("conn-1");

    ASSERT_TRUE(userId.has_value());
    EXPECT_EQ(*userId, "user-99");
}

TEST(SessionRegistryTest, RemoveClearsUserAndGameForThatConnection)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");
    registry.bindGame("conn-1", "game-7");

    registry.remove("conn-1");

    EXPECT_FALSE(registry.getUserId("conn-1").has_value());
    EXPECT_FALSE(registry.getGameId("conn-1").has_value());
}

TEST(SessionRegistryTest, RemoveUnknownConnectionDoesNotCrash)
{
    SessionRegistry registry;

    EXPECT_NO_THROW(registry.remove("no-such-connection"));
}

TEST(SessionRegistryTest, RemoveDoesNotAffectOtherConnections)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");
    registry.bindUser("conn-2", "user-99");

    registry.remove("conn-1");

    EXPECT_FALSE(registry.getUserId("conn-1").has_value());

    auto conn2User = registry.getUserId("conn-2");
    ASSERT_TRUE(conn2User.has_value());
    EXPECT_EQ(*conn2User, "user-99");
}

TEST(SessionRegistryTest, FindConnectionByUserIdReturnsBoundConnection)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");

    auto connId = registry.findConnectionByUserId("user-42");

    ASSERT_TRUE(connId.has_value());
    EXPECT_EQ(*connId, "conn-1");
}

TEST(SessionRegistryTest, FindConnectionByUserIdReturnsNulloptForUnknownUser)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");

    EXPECT_FALSE(registry.findConnectionByUserId("no-such-user").has_value());
}

TEST(SessionRegistryTest, FindConnectionByUserIdAfterRemoveReturnsNullopt)
{
    SessionRegistry registry;
    registry.bindUser("conn-1", "user-42");
    registry.remove("conn-1");

    EXPECT_FALSE(registry.findConnectionByUserId("user-42").has_value());
}