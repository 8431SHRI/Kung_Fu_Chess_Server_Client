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

void GameSession::pollSource(IPlayerSource* source, Side side)
{
    if (!source)
    {
        return;
    }

    if (source->hasPendingMove())
    {
        GameCommand cmd;
        cmd.type = GameCommandType::MOVE;
        cmd.side = side;
        source->requestMove(cmd.from, cmd.to);
        enqueue(cmd);
    }

    if (source->hasPendingJump())
    {
        GameCommand cmd;
        cmd.type = GameCommandType::JUMP;
        cmd.side = side;
        source->requestJump(cmd.at);
        enqueue(cmd);
    }
}

void GameSession::pollPlayerSources()
{
    pollSource(whiteSource_.get(), Side::WHITE);
    pollSource(blackSource_.get(), Side::BLACK);
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

void GameSession::tick(int dtMs)
{
    if (gameOver_)
    {
        return;
    }

    pollPlayerSources();
    drainQueue();

    engine_->wait(dtMs);

    GameSnapshot snap = engine_->snapshot();

    if (snap.gameOver && !gameOver_)
    {
        gameOver_ = true;

        if (gameOverCallback_)
        {
            gameOverCallback_("game_over");
        }
    }

    if (snapshotCallback_)
    {
        snapshotCallback_(snap);
    }
}