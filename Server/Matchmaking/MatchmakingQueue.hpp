#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

/*
 * @struct QueueEntry
 * @brief שחקן ממתין ל-matchmaking אקראי.
 */
struct QueueEntry
{
    std::string userId;
    std::string username;
    int elo = 1000;
    std::string connectionId;
    std::function<void(const std::string&)> sendCallback;
    std::chrono::steady_clock::time_point enqueuedAt;
};

/*
 * @class MatchmakingQueue
 * @brief רשימת ממתינים בלבד - add/remove/find-by-elo-range. לא יוצרת GameSession בעצמה.
 */
class MatchmakingQueue
{
public:
    void add(QueueEntry entry);

    void remove(const std::string& userId);

    bool contains(const std::string& userId) const;

    // מחזיר את כל ה-entries בטווח [elo-range, elo+range], לא כולל excludeUserId עצמו.
    // לא מסיר אותם מהתור - זה תפקיד הקורא (MatchmakingService) אחרי שהוחלט לזווג.
    std::vector<QueueEntry> findByEloRange(int elo, int range, const std::string& excludeUserId) const;

    // עותק מלא של כל התור - לשימוש MatchmakingService בסריקת timeout לבוט.
    std::vector<QueueEntry> snapshot() const;

    size_t size() const;

private:
    mutable std::mutex mutex_;
    std::vector<QueueEntry> entries_;
};