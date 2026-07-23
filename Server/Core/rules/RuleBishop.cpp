#include "RuleBishop.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RuleBishop::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // הגדרת ארבעת כיווני התנועה האלכסוניים של הרץ:
    // {שינוי בשורה, שינוי בעמודה} -> ימין-למטה, שמאל-למטה, ימין-למעלה, שמאל-למעלה
    const int diagonalDirections[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    
    // מעבר על כל כיוון והוספת כל המהלכים החוקיים באותו אלכסון בעזרת פונקציית העזר
    for (const auto& dir : diagonalDirections) {
        addMovesInDirection(board, piece, dir[0], dir[1], moves);
    }
    
    return moves;
}