#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "GameSession.hpp"

/*
 * @class Scheduler
 * @brief מריץ tick() על כל ה-GameSession-ים הפעילים בקצב קבוע (ברירת מחדל 16ms), ב-thread יחיד.
 * Purpose: הלב הפועם של השרת בזמן-אמת.
 * Note: registerSession/unregisterSession יכולים להיקרא מ-thread אחר (למשל Matchmaking/RoomManager) —
 *       ולכן מוגנים ב-mutex. לולאת ה-tick עצמה רצה על thread יחיד שמפעיל start().
 *       לא נוגע ב-Board/RuleEngine/GameEngine ישירות — רק דרך GameSession.
 */
class Scheduler
{
public:
    explicit Scheduler(int tickIntervalMs = 16);
    ~Scheduler();

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    // מריץ את הלולאה על thread חדש. לא חוסם את הקורא. אין-אפקט אם כבר רץ.
    void start();

    // עוצר את הלולאה ומחכה שה-thread ייסגר (join). בטוח לקרוא גם אם start() לא נקרא,
    // וגם מה-destructor.
    void stop();

    void registerSession(std::shared_ptr<GameSession> session);

    void unregisterSession(const std::string& gameId);

    bool isRunning() const { return running_; }

private:
    void run();

    int tickIntervalMs_;

    std::atomic<bool> running_{false};
    std::thread thread_;

    mutable std::mutex sessionsMutex_;
    std::unordered_map<std::string, std::shared_ptr<GameSession>> sessions_;
};