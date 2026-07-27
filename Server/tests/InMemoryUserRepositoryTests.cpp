#include <gtest/gtest.h>
#include "InMemoryUserRepository.hpp"

TEST(InMemoryUserRepositoryTest, CreateReturnsUserWithGivenUsername)
{
    InMemoryUserRepository repo;
    User u = repo.create("alice", "hashedPassword1");

    EXPECT_EQ(u.username, "alice");
    EXPECT_EQ(u.passwordHash, "hashedPassword1");
}

TEST(InMemoryUserRepositoryTest, NewUserDefaultsTo1000Elo)
{
    InMemoryUserRepository repo;
    User u = repo.create("alice", "hash1");

    EXPECT_EQ(u.elo, 1000);
}

TEST(InMemoryUserRepositoryTest, CreateAssignsDistinctIds)
{
    InMemoryUserRepository repo;
    User u1 = repo.create("alice", "hash1");
    User u2 = repo.create("bob", "hash2");

    EXPECT_NE(u1.id, u2.id);
}

TEST(InMemoryUserRepositoryTest, FindByUsernameReturnsCreatedUser)
{
    InMemoryUserRepository repo;
    repo.create("alice", "hash1");

    auto found = repo.findByUsername("alice");

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->username, "alice");
}

TEST(InMemoryUserRepositoryTest, FindByUsernameReturnsNulloptForUnknownUser)
{
    InMemoryUserRepository repo;

    auto found = repo.findByUsername("noSuchUser");

    EXPECT_FALSE(found.has_value());
}

TEST(InMemoryUserRepositoryTest, FindByIdReturnsCreatedUser)
{
    InMemoryUserRepository repo;
    User created = repo.create("alice", "hash1");

    auto found = repo.findById(created.id);

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->username, "alice");
}

TEST(InMemoryUserRepositoryTest, FindByIdReturnsNulloptForUnknownId)
{
    InMemoryUserRepository repo;

    auto found = repo.findById("no-such-id");

    EXPECT_FALSE(found.has_value());
}

TEST(InMemoryUserRepositoryTest, ExistsByUsernameTrueAfterCreate)
{
    InMemoryUserRepository repo;
    repo.create("alice", "hash1");

    EXPECT_TRUE(repo.existsByUsername("alice"));
}

TEST(InMemoryUserRepositoryTest, ExistsByUsernameFalseForUnknownUser)
{
    InMemoryUserRepository repo;

    EXPECT_FALSE(repo.existsByUsername("noSuchUser"));
}

TEST(InMemoryUserRepositoryTest, UpdateEloChangesStoredValue)
{
    InMemoryUserRepository repo;
    User created = repo.create("alice", "hash1");

    bool updated = repo.updateElo(created.id, 1250);

    ASSERT_TRUE(updated);
    auto found = repo.findById(created.id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->elo, 1250);
}

TEST(InMemoryUserRepositoryTest, UpdateEloOnMissingUserReturnsFalse)
{
    InMemoryUserRepository repo;

    EXPECT_FALSE(repo.updateElo("no-such-id", 1200));
}