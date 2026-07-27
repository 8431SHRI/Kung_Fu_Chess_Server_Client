#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "EventBus.hpp"
#include "GameEvents.hpp"

/*
 * @class MoveLogSubscriber
 * @brief מאזין ל-MoveMadeEvent/PieceCapturedEvent ושומר לוג מהלכים לכל gameId.
 * Purpose: מימוש ראשון ל-"Update Move Logs" מה-slide של ה-BUS.
 * Note: לא יודע כלום על GameSession/Room/Network - רק מאזין ל-Events. אפשר להחליף
 *       את היעד (כרגע: זיכרון) בקובץ/DB בלי לגעת ב-GameSession בכלל.
 */
class MoveLogSubscriber
{
public:
    explicit MoveLogSubscriber(std::shared_ptr<EventBus> bus)
        : bus_(std::move(bus))
    {
        moveSubId_ = bus_->subscribe<MoveMadeEvent>(
            [this](const MoveMadeEvent& e) { onMoveMade(e); });

        captureSubId_ = bus_->subscribe<PieceCapturedEvent>(
            [this](const PieceCapturedEvent& e) { onPieceCaptured(e); });
    }

    ~MoveLogSubscriber()
    {
        bus_->unsubscribe<MoveMadeEvent>(moveSubId_);
        bus_->unsubscribe<PieceCapturedEvent>(captureSubId_);
    }

    // מחזיר עותק של הלוג הנוכחי למשחק נתון (למשל כדי לצרף ל-GAME_OVER, או לדבג).
    std::vector<std::string> getLog(const std::string& gameId) const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = logs_.find(gameId);
        return (it != logs_.end()) ? it->second : std::vector<std::string>{};
    }

private:
    std::shared_ptr<EventBus> bus_;
    EventBus::SubscriptionId moveSubId_ = 0;
    EventBus::SubscriptionId captureSubId_ = 0;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::vector<std::string>> logs_;

    void onMoveMade(const MoveMadeEvent& e)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        std::string entry =
            std::string(e.side == Side::WHITE ? "W " : "B ") +
            typeToString(e.pieceType) + " " +
            std::to_string(e.from.getRow()) + "," + std::to_string(e.from.getCol()) +
            " -> " +
            std::to_string(e.to.getRow()) + "," + std::to_string(e.to.getCol());

        logs_[e.gameId].push_back(std::move(entry));
    }

    void onPieceCaptured(const PieceCapturedEvent& e)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        std::string entry =
            std::string("  x captured ") + typeToString(e.capturedPieceType) +
            " at " + std::to_string(e.at.getRow()) + "," + std::to_string(e.at.getCol());

        logs_[e.gameId].push_back(std::move(entry));
    }
};
