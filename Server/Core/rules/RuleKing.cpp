#include "RuleKing.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RuleKing::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // מעבר על כל שמונת הכיוונים האפשריים סביב המלך (טווח של 1- עד 1)
    for (int rowDelta = -1; rowDelta <= 1; ++rowDelta) {
        for (int colDelta = -1; colDelta <= 1; ++colDelta) {
            // דילוג על מצב שבו אין שינוי במיקום (רווח של 0 בשורה ו-0 בעמודה)
            if (rowDelta == 0 && colDelta == 0) continue;
            
            // חישוב מיקום היעד המבוקש
            Position targetPos(piece.getPosition().getRow() + rowDelta, piece.getPosition().getCol() + colDelta);
            
            // בדיקה שהיעד נמצא בתוך גבולות הלוח
            if (board.isInsideBoard(targetPos)) {
                auto target = board.getPieceAt(targetPos);
                
                // המהלך חוקי אם המשבצת ריקה, או אם יש בה כלי של היריב (לא מאותו צד)
                if (!target || target->getSide() != piece.getSide()) {
                    moves.insert(targetPos);
                }
            }
        }
    }
    return moves;
}