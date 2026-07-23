#pragma once
#include "IMoveRule.hpp"
#include <set>

class Board;
class Piece;
class Position;

/**
 * @class RulePawn
 * @brief מימוש חוקי התנועה של הרגלי (Pawn).
 */
class RulePawn : public IMoveRule {
public:
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;

private:
    /**
     * @brief מחשב ומוסיף את תנועות הצעד היחיד והצעד הכפול קדימה.
     */
    void addForwardMoves(const Board& board, const Piece& piece, std::set<Position>& moves) const;

    /**
     * @brief מחשב ומוסיף את אפשרויות האכילה באלכסון.
     */
    void addDiagonalCaptures(const Board& board, const Piece& piece, std::set<Position>& moves) const;
};