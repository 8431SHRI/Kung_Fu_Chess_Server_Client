#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "Board.hpp"
#include "RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "GameSnapshot.hpp"
#include "IPlayerSource.hpp"

enum class GameCommandType { MOVE, JUMP };

struct GameCommand
{
    GameCommandType type;
    Side side;
    Position from;
    Position to;
    Position at;
};

class GameSession
{
public:
    using SnapshotCallback = std::function<void(const GameSnapshot&)>;
    using GameOverCallback = std::function<void(const std::string& reason)>;

    GameSession(
        std::string gameId,
        std::unique_ptr<Board> board,
        std::unique_ptr<RuleEngine> ruleEngine,
        std::unique_ptr<RealTimeArbiter> arbiter,
        std::unique_ptr<GameEngine> engine);

    const std::string& getGameId() const { return gameId_; }

    // מחברים מקור מהלכים לכל צד. נקרא פעם אחת ע"י מי שיוצר את המשחק
    // (MatchmakingService/RoomManager) — לא ע"י GameSession עצמו.
    void setWhiteSource(std::shared_ptr<IPlayerSource> source) { whiteSource_ = std::move(source); }
    void setBlackSource(std::shared_ptr<IPlayerSource> source) { blackSource_ = std::move(source); }

    // עדיין קיים בכוונה: כניסה ישירה לתור, שימושי לבדיקות (ScriptRunner-style)
    // בלי צורך ב-IPlayerSource מלא.
    void enqueue(const GameCommand& command);

    void tick(int dtMs);

    void setSnapshotCallback(SnapshotCallback callback) { snapshotCallback_ = std::move(callback); }
    void setGameOverCallback(GameOverCallback callback) { gameOverCallback_ = std::move(callback); }

    bool isGameOver() const { return gameOver_; }

private:
    std::string gameId_;

    std::unique_ptr<Board> board_;
    std::unique_ptr<RuleEngine> ruleEngine_;
    std::unique_ptr<RealTimeArbiter> arbiter_;
    std::unique_ptr<GameEngine> engine_;

    std::shared_ptr<IPlayerSource> whiteSource_;
    std::shared_ptr<IPlayerSource> blackSource_;

    std::mutex queueMutex_;
    std::queue<GameCommand> commandQueue_;

    SnapshotCallback snapshotCallback_;
    GameOverCallback gameOverCallback_;

    bool gameOver_ = false;

    void drainQueue();

    // שואב פקודות ממתינות מ-whiteSource_/blackSource_ (אם מחוברים) לתור הפנימי.
    void pollPlayerSources();
    void pollSource(IPlayerSource* source, Side side);
};