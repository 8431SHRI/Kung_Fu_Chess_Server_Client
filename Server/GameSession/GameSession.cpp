#include "GameSession.hpp"

GameSession::GameSession(
    std::string gameId,
    std::unique_ptr<Board> board,
    std::unique_ptr<RuleEngine> ruleEngine,
    std::unique_ptr<RealTimeArbiter> arbiter,
    std::unique_ptr<GameEngine> engine)
    : gameId_(std::move(gameId))
    , board_(std::move(board))
    , ruleEngine_(std::move(ruleEngine))
    , arbiter_(std::move(arbiter))
    , engine_(std::move(engine))
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

    if (snapshotCallback_)
    {
        snapshotCallback_(engine_->snapshot());
    }

    // TODO: עדיין חסר isGameOver() אמיתי ב-GameEngine - ראו הערה קודמת (KnownGap).
}