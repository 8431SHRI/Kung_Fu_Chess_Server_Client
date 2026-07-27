#include "GreedyCaptureBotStrategy.hpp"

#include <algorithm>
#include <random>
#include <vector>

bool GreedyCaptureBotStrategy::tryDecideMove(
    Side side,
    const Board& board,
    const RuleEngine& ruleEngine,
    Position& outFrom,
    Position& outTo) const
{
    std::vector<Position> ownPiecePositions;

    for (int r = 0; r < board.getRows(); ++r)
    {
        for (int c = 0; c < board.getCols(); ++c)
        {
            Position from(r, c);
            auto piece = board.getPieceAt(from);

            if (piece && piece->getSide() == side)
            {
                ownPiecePositions.push_back(from);
            }
        }
    }

    static thread_local std::mt19937 rng(std::random_device{}());
    std::shuffle(ownPiecePositions.begin(), ownPiecePositions.end(), rng);

    // מעבר ראשון: מחפשים מהלך שאוכל כלי יריב (יש כלי יריב ביעד).
    for (const auto& from : ownPiecePositions)
    {
        auto legalMoves = ruleEngine.getLegalMoves(from, board);

        std::vector<Position> captureMoves;
        for (const auto& to : legalMoves)
        {
            auto targetPiece = board.getPieceAt(to);
            if (targetPiece && targetPiece->getSide() != side)
            {
                captureMoves.push_back(to);
            }
        }

        if (!captureMoves.empty())
        {
            std::uniform_int_distribution<size_t> dist(0, captureMoves.size() - 1);
            outFrom = from;
            outTo = captureMoves[dist(rng)];
            return true;
        }
    }

    // מעבר שני: אין אכילה זמינה לאף כלי - fallback למהלך חוקי רנדומלי, כמו הגרסה המקורית.
    for (const auto& from : ownPiecePositions)
    {
        auto legalMoves = ruleEngine.getLegalMoves(from, board);

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
