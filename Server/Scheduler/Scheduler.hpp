#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "GameSession.hpp"

/*
 * @class Scheduler
 * @brief מריץ tick() על כל ה-GameSession-ים הפעילים בקצב קבוע, על thread יחיד.
 * Purpose: לולאת הזמן המרכזית של השרת. לא נוגע ישירות ב-Board/RuleEngine —
 *          רק קורא ל-GameSession::tick(), שהוא היחיד שמכיר את ה-Core.
 * Note: register/unregister הם thread-safe (יכולים להיקרא מ-thread של Network),
 *       אבל tick() עצמו רץ תמיד על thread הפנימי היחיד של ה-Scheduler.
 */
class Scheduler
{
public:
    explicit Scheduler(int tickIntervalMs = 16);

    ~Scheduler();

    // מתחיל thread פנימי שרץ בלולאה עד stop(). לא-חוסם.
    void run();

    // עוצר את הלולאה ומחכה ל-thread הפנימי לסיים (join). בטוח לקרוא פעמיים.
    void stop();

    // thread-safe. אפשר לקרוא בזמן שה-Scheduler רץ.
    void registerSession(std::shared_ptr<GameSession> session);

    // thread-safe. מסיר session לפי gameId (למשל בסיום משחק).
    void unregisterSession(const std::string& gameId);

    bool isRunning() const { return running_.load(); }

private:
    int tickIntervalMs_;

    std::atomic<bool> running_{false};

    std::thread workerThread_;

    mutable std::mutex sessionsMutex_;

    std::unordered_map<std::string, std::shared_ptr<GameSession>> sessions_;

    void loop();

    std::vector<std::shared_ptr<GameSession>> snapshotActiveSessions() const;
};