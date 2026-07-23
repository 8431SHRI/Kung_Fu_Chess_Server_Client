#include "RuleQueen.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RuleQueen::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // הגדרת 8 כיווני התנועה האפשריים של המלכה:
    // 4 כיוונים ישרים (כמו צריח) ו-4 כיוונים אלכסוניים (כמו רץ)
    const int allDirections[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},  // למטה, למעלה, ימינה, שמאלה
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // ארבעת האלכסונים
    };
    
    // מעבר על כל כיוון והוספת כל המהלכים החוקיים באותו מסלול בעזרת פונקציית העזר
    for (const auto& dir : allDirections) {
        addMovesInDirection(board, piece, dir[0], dir[1], moves);
    }
    
    return moves;
}