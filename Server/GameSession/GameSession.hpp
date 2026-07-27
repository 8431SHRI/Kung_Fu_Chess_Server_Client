#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

#include "Board.hpp"
#include "RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "GameSnapshot.hpp"
#include "IPlayerSource.hpp"
#include "EventBus.hpp"

enum class GameCommandType { MOVE, JUMP };

struct GameCommand
{
    GameCommandType type;
    Side side;
    Position from; // תקף רק ל-MOVE
    Position to;   // תקף רק ל-MOVE
    Position at;   // תקף רק ל-JUMP
};

/*
 * @class GameSession
 * @brief עוטף מופע יחיד של Board+RuleEngine+GameEngine+RealTimeArbiter הקיימים (ללא שינוי בהם).
 * Purpose: מנהל תור פקודות נכנס ומשדר GameSnapshot ליציאה בכל tick.
 * Note: two input paths - enqueue() ישיר (למשל מ-MoveCommand/Application), וגם
 *       IPlayerSource אופציונלי לכל צד (נסקר ב-tick אם רשום) - בעיקר בשביל BotPlayerSource,
 *       כדי ש-Room יוכל לרשום בוט בלי ש-Application יצטרך "לדחוף" עבורו מהלכים בכל tick.
 */
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
        std::unique_ptr<GameEngine> engine,
        std::shared_ptr<EventBus> bus = nullptr);

    const std::string& getGameId() const { return gameId_; }

    // אופציונלי: אם לא הועבר ל-constructor, אפשר לחבר מאוחר יותר (למשל ע"י Room,
    // מיד אחרי היצירה, לפני הרישום ל-Scheduler).
    void setEventBus(std::shared_ptr<EventBus> bus) { bus_ = std::move(bus); }

    void enqueue(const GameCommand& command);

    // אופציונלי: רושם IPlayerSource לצד מסוין - ייסקר בכל tick() בנוסף לתור.
    // בעיקר לשימוש BotPlayerSource; לשחקן אנושי אפשר גם דרך enqueue() ישיר.
    void setPlayerSource(Side side, std::shared_ptr<IPlayerSource> source);

    void tick(int dtMs);

    void setSnapshotCallback(SnapshotCallback callback) { snapshotCallback_ = std::move(callback); }

    void setGameOverCallback(GameOverCallback callback) { gameOverCallback_ = std::move(callback); }
    // הפסד אוטומטי חיצוני (ניתוק/ויתור) - נקרא ע"י Room, לא ע"י GameEngine/Core.
    // לא נוגעת ב-Board/RuleEngine בכלל - רק מסמנת gameOver_ ומפרסמת GameOverEvent.
    // אם המשחק כבר הסתיים (למשל מלך נאכל באותו tick) - no-op, לא דורסת תוצאה קודמת.
    void forceGameOver(Side loserSide, const std::string& reason);
    bool isGameOver() const { return gameOver_; }

    // חשיפת Board/RuleEngine ל-BotPlayerSource שנבנה *אחרי* ש-GameSession כבר קיים
    // (למשל ע"י Room::fillWithBot). בטוח - GameSession מחזיק אותם למשך כל חייו.
    Board& getBoard() const { return *board_; }
    RuleEngine& getRuleEngine() const { return *ruleEngine_; }

private:
    std::string gameId_;

    std::unique_ptr<Board> board_;
    std::unique_ptr<RuleEngine> ruleEngine_;
    std::unique_ptr<RealTimeArbiter> arbiter_;
    std::unique_ptr<GameEngine> engine_;

    std::mutex queueMutex_;
    std::queue<GameCommand> commandQueue_;

    std::shared_ptr<IPlayerSource> whiteSource_;
    std::shared_ptr<IPlayerSource> blackSource_;

    SnapshotCallback snapshotCallback_;
    GameOverCallback gameOverCallback_;

    bool gameOver_ = false;

    std::shared_ptr<EventBus> bus_; // אופציונלי - כל publish מוגן ב-if (bus_)

    // מצב הכלים בסוף ה-tick הקודם, כדי לזהות מה "השתנה בפועל" (הושלם מהלך / כלי נאכל)
    // בלי לגעת ב-GameEngine/RealTimeArbiter בכלל - הכל מבוסס על ה-snapshot הציבורי הקיים.
    bool hasPreviousSnapshot_ = false;
    std::unordered_map<int, PieceSnapshot> previousPiecesById_;

    void drainQueue();
    void pollPlayerSource(const std::shared_ptr<IPlayerSource>& source);
    void publishSnapshotDiffEvents(const GameSnapshot& snapshot);
};