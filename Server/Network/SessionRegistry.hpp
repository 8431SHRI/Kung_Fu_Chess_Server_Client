#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

/*
 * @class SessionRegistry
 * @brief ממפה connectionId <-> userId <-> gameId.
 * Purpose: מאפשר לדעת "מי מחובר עכשיו ולאיזה משחק", לצורך ניתוב MOVE/JUMP עתידי
 *          (MoveCommand) ולצורך reconnect.
 * Note: thread-safe בסיסי (mutex יחיד). אין כאן שום לוגיקת משחק.
 */
class SessionRegistry
{
public:
    void bindUser(const std::string& connectionId, const std::string& userId);

    void bindGame(const std::string& connectionId, const std::string& gameId);

    std::optional<std::string> getUserId(const std::string& connectionId) const;

    std::optional<std::string> getGameId(const std::string& connectionId) const;

    // נקרא בכל disconnect - מנקה את כל המידע על החיבור הזה.
    void remove(const std::string& connectionId);

    // לצורך reconnect: איתור connectionId פעיל (אם יש) לפי userId.
    std::optional<std::string> findConnectionByUserId(const std::string& userId) const;

private:
    mutable std::mutex mutex_;

    struct SessionInfo
    {
        std::optional<std::string> userId;
        std::optional<std::string> gameId;
    };

    std::unordered_map<std::string, SessionInfo> connections_;
};