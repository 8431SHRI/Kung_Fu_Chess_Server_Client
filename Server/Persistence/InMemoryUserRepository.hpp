#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

#include "IUserRepository.hpp"

/*
 * @class InMemoryUserRepository
 * @brief מימוש In-Memory של IUserRepository, מעל std::unordered_map.
 * Purpose: מאפשר לבנות ולבדוק את זרימת ה-Auth מקצה לקצה לפני שיש חיבור SQLite.
 * Note: thread-safe באמצעות mutex יחיד; זמני/לבדיקות, לא מיועד לאחסון production.
 */
class InMemoryUserRepository : public IUserRepository
{
public:
    InMemoryUserRepository() = default;

    std::optional<User> findByUsername(const std::string& username) const override;

    std::optional<User> findById(const std::string& id) const override;

    bool existsByUsername(const std::string& username) const override;

    User create(const std::string& username, const std::string& passwordHash) override;

    bool updateElo(const std::string& id, int newElo) override;

private:
    mutable std::mutex mutex_;

    std::unordered_map<std::string, User> usersById_;

    std::unordered_map<std::string, std::string> usernameToId_;

    int nextId_ = 1;

    std::string generateId();
};