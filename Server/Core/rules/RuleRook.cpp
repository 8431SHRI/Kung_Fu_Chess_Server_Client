#include "RuleRook.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RuleRook::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // הגדרת ארבעת כיווני התנועה האורתוגונליים (הישרים) של הצריח:
    // {שינוי בשורה, שינוי בעמודה} -> למטה, למעלה, ימינה, שמאלה
    const int orthogonalDirections[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    
    // מעבר על כל כיוון והוספת כל המהלכים החוקיים באותו מסלול בעזרת פונקציית העזר
    for (const auto& dir : orthogonalDirections) {
        addMovesInDirection(board, piece, dir[0], dir[1], moves);
    }
    
    return moves;
}