#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "MatchmakingQueue.hpp"
#include "RoomManager.hpp"
#include "SessionRegistry.hpp"

/*
 * @class MatchmakingService
 * @brief סריקה תקופתית של MatchmakingQueue: זיווג לפי ELO±eloRange_, fallback לבוט
 *        אחרי botFallbackMs_ בלי יריב.
 * Purpose: יוצרת "חדר" (דרך RoomManager, בשם ייחודי) לכל התאמה - לא בונה GameSession
 *          בעצמה, כדי לא לשכפל את הלוגיקה שכבר קיימת ב-Room.
 * Note: לא מחשבת ELO סופי בעצמה - זה EloCalculator, ורק אחרי סיום משחק אמיתי
 *       (TODO: GameSession::GameOverCallback עדיין לא מחובר בפועל - ראו KnownGap).
 */
class MatchmakingService
{
public:
    MatchmakingService(
        std::shared_ptr<RoomManager> roomManager,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        int eloRange = 100,
        int scanIntervalMs = 500,
        int botFallbackMs = 8000);

    ~MatchmakingService();

    void enqueue(
        const std::string& userId,
        const std::string& username,
        int elo,
        const std::string& connectionId,
        std::function<void(const std::string&)> sendCallback);

    void cancel(const std::string& userId);

    void run();
    void stop();

    bool isRunning() const { return running_.load(); }

private:
    std::shared_ptr<RoomManager> roomManager_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    MatchmakingQueue queue_;

    int eloRange_;
    int scanIntervalMs_;
    int botFallbackMs_;

    std::atomic<bool> running_{false};
    std::thread workerThread_;
    std::atomic<uint64_t> nextRoomId_{1};

    void loop();
    void scanOnce();
    void createMatch(const QueueEntry& a, const QueueEntry& b);
    void createBotMatch(const QueueEntry& a);
    std::string generateRoomName();
};