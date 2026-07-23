#include "RuleUtils.hpp"
#include "Board.hpp"
#include "Piece.hpp"

void addMovesInDirection(const Board& board, const Piece& piece, int rowStep, int colStep, std::set<Position>& legalMoves) {
    Position currentPos = piece.getPosition();
    int nextRow = currentPos.getRow() + rowStep;
    int nextCol = currentPos.getCol() + colStep;

    // לולאה הרצה כל עוד המיקום הבא נמצא בתוך גבולות הלוח
    while (board.isInsideBoard(Position(nextRow, nextCol))) {
        Position targetPos(nextRow, nextCol);
        auto targetPiece = board.getPieceAt(targetPos);

        if (!targetPiece) {
            // משבצת ריקה - מהלך חוקי, ממשיכים לסרוק בכיוון זה
            legalMoves.insert(targetPos);
        } else {
            // משבצת תפוסה - אם זה כלי של היריב, ניתן לאכול אותו
            if (targetPiece->getSide() != piece.getSide()) {
                legalMoves.insert(targetPos);
            }
            break; // עצירה בכל מקרה של חסימה (אי אפשר לדלג מעל כלי אחר)
        }
        nextRow += rowStep;
        nextCol += colStep;
    }
}