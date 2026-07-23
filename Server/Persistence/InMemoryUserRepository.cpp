#include "InMemoryUserRepository.hpp"

std::optional<User> InMemoryUserRepository::findByUsername(const std::string& username) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = usernameToId_.find(username);

    if (it == usernameToId_.end())
    {
        return std::nullopt;
    }

    return usersById_.at(it->second);
}

std::optional<User> InMemoryUserRepository::findById(const std::string& id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = usersById_.find(id);

    if (it == usersById_.end())
    {
        return std::nullopt;
    }

    return it->second;
}

bool InMemoryUserRepository::existsByUsername(const std::string& username) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return usernameToId_.find(username) != usernameToId_.end();
}

User InMemoryUserRepository::create(const std::string& username, const std::string& passwordHash)
{
    std::lock_guard<std::mutex> lock(mutex_);

    User user;
    user.id = generateId();
    user.username = username;
    user.passwordHash = passwordHash;
    user.elo = 1000;

    usersById_[user.id] = user;
    usernameToId_[username] = user.id;

    return user;
}

bool InMemoryUserRepository::updateElo(const std::string& id, int newElo)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = usersById_.find(id);

    if (it == usersById_.end())
    {
        return false;
    }

    it->second.elo = newElo;
    return true;
}

std::string InMemoryUserRepository::generateId()
{
    // הערה: מזהה סידורי בזיכרון בלבד. כשנעבור ל-SQLite, ה-id ייווצר ע"י ה-DB (AUTOINCREMENT/UUID).
    return std::to_string(nextId_++);
}