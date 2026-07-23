#include "RuleKnight.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RuleKnight::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // הגדרת 8 הקפיצות האפשריות של הפרש (צורת L)
    const int knightJumps[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, 
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
    
    for (const auto& jump : knightJumps) {
        Position targetPos(piece.getPosition().getRow() + jump[0], piece.getPosition().getCol() + jump[1]);
        
        // בדיקה שהיעד נמצא בתוך גבולות הלוח
        if (board.isInsideBoard(targetPos)) {
            auto target = board.getPieceAt(targetPos);
            
            // המהלך חוקי אם המשבצת ריקה או אם נמצא בה כלי של היריב (לא מאותו צד)
            if (!target || target->getSide() != piece.getSide()) {
                moves.insert(targetPos);
            }
        }
    }
    return moves;
}