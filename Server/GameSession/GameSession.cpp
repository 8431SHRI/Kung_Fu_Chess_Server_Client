#include "GameSession.hpp"

#include "GameEvents.hpp"

GameSession::GameSession(
    std::string gameId,
    std::unique_ptr<Board> board,
    std::unique_ptr<RuleEngine> ruleEngine,
    std::unique_ptr<RealTimeArbiter> arbiter,
    std::unique_ptr<GameEngine> engine,
    std::shared_ptr<EventBus> bus)
    : gameId_(std::move(gameId))
    , board_(std::move(board))
    , ruleEngine_(std::move(ruleEngine))
    , arbiter_(std::move(arbiter))
    , engine_(std::move(engine))
    , bus_(std::move(bus))
{
}

void GameSession::enqueue(const GameCommand& command)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    commandQueue_.push(command);
}

void GameSession::setPlayerSource(Side side, std::shared_ptr<IPlayerSource> source)
{
    if (side == Side::WHITE) { whiteSource_ = std::move(source); }
    else { blackSource_ = std::move(source); }
}

void GameSession::drainQueue()
{
    std::queue<GameCommand> localQueue;

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        std::swap(localQueue, commandQueue_);
    }

    while (!localQueue.empty())
    {
        const GameCommand& cmd = localQueue.front();

        if (cmd.type == GameCommandType::MOVE)
        {
            engine_->requestMove(cmd.from, cmd.to);
        }
        else
        {
            engine_->requestJump(cmd.at);
        }

        localQueue.pop();
    }
}

void GameSession::pollPlayerSource(const std::shared_ptr<IPlayerSource>& source)
{
    if (!source)
    {
        return;
    }

    if (source->hasPendingMove())
    {
        Position from, to;
        source->requestMove(from, to);
        engine_->requestMove(from, to);
    }

    if (source->hasPendingJump())
    {
        Position at;
        source->requestJump(at);
        engine_->requestJump(at);
    }
}

void GameSession::tick(int dtMs)
{
    if (gameOver_)
    {
        return;
    }

    drainQueue();

    pollPlayerSource(whiteSource_);
    pollPlayerSource(blackSource_);

    engine_->wait(dtMs);

    const GameSnapshot snapshot = engine_->snapshot();

    if (snapshotCallback_)
    {
        snapshotCallback_(snapshot);
    }

    publishSnapshotDiffEvents(snapshot);

    // KnownGap שתוקן: GameEngine::signalGameOver() כבר קורה בפנים (ראו RealTimeArbiter::
    // executeStandardMove כשנאכל מלך), אבל עד עכשיו GameSession אף פעם לא בדק את
    // snapshot.gameOver ולכן gameOverCallback_ מעולם לא נקרא בפועל. עכשיו כן.
    if (snapshot.gameOver && !gameOver_)
    {
        gameOver_ = true;

        // מנחשים את המנצח לפי איזה מלך נעלם ב-diff (ראו publishSnapshotDiffEvents) -
        // אם לא הצלחנו לזהות (למקרה קצה), משאירים ל-nullopt = "לא ידוע/תיקו".
        if (gameOverCallback_)
        {
            gameOverCallback_("king_captured");
        }
    }
}

void GameSession::publishSnapshotDiffEvents(const GameSnapshot& snapshot)
{
    std::unordered_map<int, PieceSnapshot> currentById;
    currentById.reserve(snapshot.pieces.size());

    for (const auto& piece : snapshot.pieces)
    {
        currentById[piece.id] = piece;
    }

    if (hasPreviousSnapshot_)
    {
        // מהלכים שהושלמו ב-tick הזה: כלי שהמיקום שלו השתנה מאז ה-snapshot הקודם,
        // ושכרגע אינו "באמצע תנועה" (כלומר: התנועה כבר הסתיימה, לא רק התקדמה).
        for (const auto& [id, current] : currentById)
        {
            auto it = previousPiecesById_.find(id);

            if (it == previousPiecesById_.end())
            {
                continue; // כלי חדש (למשל הופיע רק עכשיו) - לא "זז", דלג
            }

            const PieceSnapshot& previous = it->second;

            if (!current.isMoving && !(previous.boardPosition == current.boardPosition))
            {
                if (bus_)
                {
                    MoveMadeEvent event{gameId_, current.side, current.type, previous.boardPosition, current.boardPosition};
                    bus_->publish(event);
                }
            }
        }

        // כלים שנעלמו לגמרי מאז ה-snapshot הקודם = נאכלו ב-tick הזה.
        std::optional<Side> capturedSideOfKing;

        for (const auto& [id, previous] : previousPiecesById_)
        {
            if (currentById.find(id) != currentById.end())
            {
                continue; // עדיין קיים - לא נאכל
            }

            if (previous.type == PieceType::KING)
            {
                capturedSideOfKing = previous.side;
            }

            if (bus_)
            {
                // capturingSide משוער: הצד שהמלך שלו לא נאכל. במקרה של כלי רגיל (לא מלך)
                // אין לנו כאן, ברמת ה-diff, ודאות מלאה מי אכל - זה מספיק ל-log/sound.
                Side capturingSide = (previous.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
                PieceCapturedEvent event{gameId_, capturingSide, previous.type, previous.boardPosition};
                bus_->publish(event);
            }
        }

        if (snapshot.gameOver && bus_)
        {
            GameOverEvent event;
            event.gameId = gameId_;
            event.reason = "king_captured";
            event.winnerSide = capturedSideOfKing.has_value()
                ? std::optional<Side>((*capturedSideOfKing == Side::WHITE) ? Side::BLACK : Side::WHITE)
                : std::nullopt;

            bus_->publish(event);
        }
    }

    previousPiecesById_ = std::move(currentById);
    hasPreviousSnapshot_ = true;
}
void GameSession::forceGameOver(Side loserSide, const std::string& reason)
{
    if (gameOver_)
    {
        return;
    }

    gameOver_ = true;

    if (bus_)
    {
        GameOverEvent event;
        event.gameId = gameId_;
        event.winnerSide = (loserSide == Side::WHITE) ? Side::BLACK : Side::WHITE;
        event.reason = reason;
        bus_->publish(event);
    }

    if (gameOverCallback_)
    {
        gameOverCallback_(reason);
    }
}