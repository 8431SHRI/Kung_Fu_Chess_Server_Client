#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "EloCalculator.hpp"
#include "EventBus.hpp"
#include "GameEvents.hpp"
#include "IUserRepository.hpp"

/*
 * @class ScoreUpdateSubscriber
 * @brief מימוש "Update Scores" מה-slide של ה-BUS: על GameOverEvent, מחשב ELO חדש
 *        (EloCalculator הקיים) ושומר אותו דרך IUserRepository הקיים - בלי ש-Room
 *        או GameSession יצטרכו להכיר את אף אחד מהשניים.
 * Usage: ה-caller (Room, כשנוצר משחק בין שני שחקנים אנושיים) קורא ל-registerGame()
 *        פעם אחת עם ה-userId/elo של שני הצדדים; ה-subscriber עצמו מחכה ל-GameOverEvent
 *        התואם ל-gameId ומטפל בהמשך לבד.
 * Note: משחקים מול בוט (רק שחקן אנושי אחד) פשוט לא נרשמים דרך registerGame - אין
 *       userId לבוט, ולכן אין עדכון ELO עבורם (החלטה מכוונת, לא פער).
 */
class ScoreUpdateSubscriber
{
public:
    ScoreUpdateSubscriber(std::shared_ptr<EventBus> bus, std::shared_ptr<IUserRepository> userRepository)
        : bus_(std::move(bus))
        , userRepository_(std::move(userRepository))
    {
        subId_ = bus_->subscribe<GameOverEvent>(
            [this](const GameOverEvent& e) { onGameOver(e); });
    }

    ~ScoreUpdateSubscriber()
    {
        bus_->unsubscribe<GameOverEvent>(subId_);
    }

    struct PlayerRatingInfo
    {
        std::string userId;
        std::string username;
        int elo;
        Side side;
    };

    void registerGame(const std::string& gameId, PlayerRatingInfo white, PlayerRatingInfo black)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingGames_[gameId] = PendingGame{std::move(white), std::move(black)};
    }

private:
    struct PendingGame
    {
        PlayerRatingInfo white;
        PlayerRatingInfo black;
    };

    std::shared_ptr<EventBus> bus_;
    std::shared_ptr<IUserRepository> userRepository_;
    EventBus::SubscriptionId subId_ = 0;

    std::mutex mutex_;
    std::unordered_map<std::string, PendingGame> pendingGames_;

    void onGameOver(const GameOverEvent& event)
    {
        PendingGame game;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            auto it = pendingGames_.find(event.gameId);
            if (it == pendingGames_.end())
            {
                return; // לא היה רשום (למשל משחק מול בוט) - אין ELO לעדכן
            }

            game = it->second;
            pendingGames_.erase(it);
        }

        MatchResult result = MatchResult::DRAW;

        if (event.winnerSide.has_value())
        {
            result = (*event.winnerSide == Side::WHITE) ? MatchResult::A_WINS : MatchResult::B_WINS;
        }

        auto [newWhiteElo, newBlackElo] = EloCalculator::newElo(game.white.elo, game.black.elo, result);

        userRepository_->updateElo(game.white.userId, newWhiteElo);
        userRepository_->updateElo(game.black.userId, newBlackElo);

        bus_->publish(ScoreUpdatedEvent{event.gameId, game.white.userId, game.white.username, game.white.elo, newWhiteElo});
        bus_->publish(ScoreUpdatedEvent{event.gameId, game.black.userId, game.black.username, game.black.elo, newBlackElo});
    }
};
