#include "SessionRegistry.hpp"

void SessionRegistry::bindUser(const std::string& connectionId, const std::string& userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    connections_[connectionId].userId = userId;
}

void SessionRegistry::bindGame(const std::string& connectionId, const std::string& gameId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    connections_[connectionId].gameId = gameId;
}

std::optional<std::string> SessionRegistry::getUserId(const std::string& connectionId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = connections_.find(connectionId);
    if (it == connections_.end()) { return std::nullopt; }
    return it->second.userId;
}

std::optional<std::string> SessionRegistry::getGameId(const std::string& connectionId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = connections_.find(connectionId);
    if (it == connections_.end()) { return std::nullopt; }
    return it->second.gameId;
}

void SessionRegistry::remove(const std::string& connectionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(connectionId);
}

std::optional<std::string> SessionRegistry::findConnectionByUserId(const std::string& userId) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& [connId, info] : connections_)
    {
        if (info.userId.has_value() && info.userId.value() == userId)
        {
            return connId;
        }
    }

    return std::nullopt;
}