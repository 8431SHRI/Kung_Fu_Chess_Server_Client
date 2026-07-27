#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameSession.hpp"
#include "NetworkPlayerSource.hpp"
#include "SpectatorSession.hpp"
#include "Scheduler.hpp"
#include "PiecePhysicsManager.hpp"
#include "EventBus.hpp"
#include "MoveLogSubscriber.hpp"
#include "ScoreUpdateSubscriber.hpp"
#include "IUserRepository.hpp"

enum class JoinRole { PLAYER, SPECTATOR };

struct JoinResult
{
    JoinRole role;
    Side side = Side::WHITE;
    std::shared_ptr<NetworkPlayerSource> playerSource;
};

class Room
{
public:
    using SendCallback = std::function<void(const std::string& rawJson)>;

    Room(
        std::string name,
        std::shared_ptr<Scheduler> scheduler,
        std::shared_ptr<PiecePhysicsManager> physicsManager,
        std::shared_ptr<IUserRepository> userRepository = nullptr);

    JoinResult join(
        const std::string& connectionId,
        const std::string& userId,
        const std::string& username,
        int elo,
        SendCallback sendCallback);

    // ה-Bus הפנימי של החדר - MOVE_MADE/PIECE_CAPTURED/GAME_STARTED/GAME_OVER מפורסמים עליו.
    // חשוף החוצה כדי ש-Network/Application יוכלו להירשם (למשל לשלוח GAME_EVENT/GAME_OVER ללקוח).
    std::shared_ptr<EventBus> getEventBus() const { return bus_; }

    // ל-MatchmakingService: ממלאת את המקום השני בבוט (אחרי timeout בלי יריב אנושי).
    // דורשת ששחקן אנושי אחד בדיוק כבר הצטרף.
    void fillWithBot();

    void leave(const std::string& connectionId);

    void broadcast(const std::string& rawJson) const;

    const std::string& getName() const { return name_; }

    bool isFull() const;

    std::shared_ptr<GameSession> getGameSession() const { return session_; }

    std::shared_ptr<NetworkPlayerSource> findPlayerSource(const std::string& connectionId) const;

private:
    struct PlayerSlot
    {
        std::string connectionId;
        std::string userId;
        std::string username;
        int elo = 1000;
        Side side;
        std::shared_ptr<NetworkPlayerSource> source;
        SendCallback sendCallback;
    };

    std::string name_;
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<PiecePhysicsManager> physicsManager_;
    std::shared_ptr<IUserRepository> userRepository_;

    std::shared_ptr<EventBus> bus_;
    std::unique_ptr<MoveLogSubscriber> moveLogSubscriber_;
    std::unique_ptr<ScoreUpdateSubscriber> scoreUpdateSubscriber_; // null אם לא הועבר repository

    mutable std::mutex mutex_;

    std::vector<PlayerSlot> players_; // גודל מקסימלי 2
    std::vector<std::unique_ptr<SpectatorSession>> spectators_;

    std::shared_ptr<GameSession> session_;

    void ensureSessionCreatedLocked(); // בונה board/session פעם ראשונה בלבד (idempotent)
    void createGameSessionLocked();    // קורא ל-ensureSessionCreatedLocked + מחבר שחקן שני אנושי
    void broadcastRoomStateLocked() const;
    void publishGameStartedLocked(const std::string& blackUsername) const;

    static std::unique_ptr<Board> buildStandardBoard();
};