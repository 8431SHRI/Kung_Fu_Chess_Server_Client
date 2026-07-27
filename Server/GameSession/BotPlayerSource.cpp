#include "BotPlayerSource.hpp"

BotPlayerSource::BotPlayerSource(
    Side side,
    const RuleEngine& ruleEngine,
    const Board& board,
    std::shared_ptr<IBotStrategy> strategy)
    : side_(side), ruleEngine_(ruleEngine), board_(board), strategy_(std::move(strategy))
{
}

bool BotPlayerSource::hasPendingMove() const
{
    Position from, to;
    return strategy_->tryDecideMove(side_, board_, ruleEngine_, from, to);
}

void BotPlayerSource::requestMove(Position& outFrom, Position& outTo)
{
    strategy_->tryDecideMove(side_, board_, ruleEngine_, outFrom, outTo);
}

bool BotPlayerSource::hasPendingJump() const
{
    // רק fallback אחרון: אין קפיצה כל עוד יש מהלך רגיל זמין.
    Position from, to;
    if (strategy_->tryDecideMove(side_, board_, ruleEngine_, from, to))
    {
        return false;
    }

    Position dummy;
    return tryPickJumpableIdlePiece(dummy);
}

void BotPlayerSource::requestJump(Position& outPosition)
{
    tryPickJumpableIdlePiece(outPosition);
}

bool BotPlayerSource::tryPickJumpableIdlePiece(Position& outPosition) const
{
    for (int r = 0; r < board_.getRows(); ++r)
    {
        for (int c = 0; c < board_.getCols(); ++c)
        {
            Position pos(r, c);
            auto piece = board_.getPieceAt(pos);

            if (!piece || piece->getSide() != side_)
            {
                continue;
            }

            if (ruleEngine_.isValidJump(piece).is_valid)
            {
                outPosition = pos;
                return true;
            }
        }
    }

    return false;
}
