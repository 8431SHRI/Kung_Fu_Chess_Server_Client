#include "BotPlayerSource.hpp"

#include <vector>
#include <random>

BotPlayerSource::BotPlayerSource(Side side, const RuleEngine& ruleEngine, const Board& board)
    : side_(side), ruleEngine_(ruleEngine), board_(board)
{
}

bool BotPlayerSource::hasPendingMove() const
{
    Position from, to;
    return tryPickRandomLegalMove(from, to);
}

void BotPlayerSource::requestMove(Position& outFrom, Position& outTo)
{
    tryPickRandomLegalMove(outFrom, outTo);
}

bool BotPlayerSource::hasPendingJump() const
{
    // הבוט לא מבצע קפיצות ב-v1 — ראה הערה ב-header
    return false;
}

void BotPlayerSource::requestJump(Position& /*outPosition*/)
{
    // no-op בכוונה: hasPendingJump() תמיד false, GameSession לא אמור לקרוא לזה
}

bool BotPlayerSource::tryPickRandomLegalMove(Position& outFrom, Position& outTo) const
{
    std::vector<Position> candidates;

    for (int r = 0; r < board_.getRows(); ++r)
    {
        for (int c = 0; c < board_.getCols(); ++c)
        {
            Position from(r, c);
            auto piece = board_.getPieceAt(from);

            if (!piece || piece->getSide() != side_)
            {
                continue;
            }

            candidates.push_back(from);
        }
    }

    static thread_local std::mt19937 rng(std::random_device{}());
    std::shuffle(candidates.begin(), candidates.end(), rng);

    for (const auto& from : candidates)
    {
        auto legalMoves = ruleEngine_.getLegalMoves(from, board_);

        if (legalMoves.empty())
        {
            continue;
        }

        std::vector<Position> moves(legalMoves.begin(), legalMoves.end());
        std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);

        outFrom = from;
        outTo = moves[dist(rng)];
        return true;
    }

    return false; // אין אף מהלך חוקי לצד הזה כרגע
}