#pragma once

#include <optional>
#include <string>

#include "User.hpp"

/*
 * @interface IUserRepository
 * @brief ממשק CRUD מופשט למשתמשים.
 * Purpose: מנתק את AuthService ממנגנון האחסון הקונקרטי (In-Memory / SQLite / ...).
 * Implemented by: InMemoryUserRepository, SQLiteUserRepository (שלב הבא).
 * Used by: AuthService.
 */
class IUserRepository
{
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<User> findByUsername(const std::string& username) const = 0;

    virtual std::optional<User> findById(const std::string& id) const = 0;

    virtual bool existsByUsername(const std::string& username) const = 0;

    virtual User create(const std::string& username, const std::string& passwordHash) = 0;

    virtual bool updateElo(const std::string& id, int newElo) = 0;
};