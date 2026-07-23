#include "RulePawn.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Position.hpp"
#include "RuleUtils.hpp"

std::set<Position> RulePawn::getLegalDestinations(const Board& board, const Piece& piece) const {
    std::set<Position> moves;
    
    // קריאה לפונקציות העזר הייעודיות - פונקציית העל כעת נקייה וברורה מאוד לקריאה
    addForwardMoves(board, piece, moves);
    addDiagonalCaptures(board, piece, moves);
    
    return moves;
}

void RulePawn::addForwardMoves(const Board& board, const Piece& piece, std::set<Position>& moves) const {
    int r = piece.getPosition().getRow();
    int c = piece.getPosition().getCol();
    int rows = board.getRows();
    
    int forwardStep = (piece.getSide() == Side::WHITE) ? -1 : 1;
    int startRow = (piece.getSide() == Side::WHITE) ? rows - 2 : 1;
    
    // 1. צעד יחיד קדימה
    Position forwardPos(r + forwardStep, c);
    if (board.isInsideBoard(forwardPos) && !board.getPieceAt(forwardPos)) {
        moves.insert(forwardPos);

        // 2. צעד כפול קדימה (מתאפשר רק אם הצעד היחיד פנוי וגם אנחנו בשורת ההתחלה)
        Position doubleStepPos(r + (2 * forwardStep), c);
        if (r == startRow && board.isInsideBoard(doubleStepPos) && !board.getPieceAt(doubleStepPos)) {
            moves.insert(doubleStepPos);
        }
    }
}

void RulePawn::addDiagonalCaptures(const Board& board, const Piece& piece, std::set<Position>& moves) const {
    int r = piece.getPosition().getRow();
    int c = piece.getPosition().getCol();
    
    int forwardStep = (piece.getSide() == Side::WHITE) ? -1 : 1;
    
    // אכילה באלכסון (שמאל וימין)
    for (int sideDelta : {-1, 1}) {
        Position attackPos(r + forwardStep, c + sideDelta);
        if (board.isInsideBoard(attackPos)) {
            auto target = board.getPieceAt(attackPos);
            // מותר לנוע לאלכסון רק אם יש שם כלי של היריב
            if (target && target->getSide() != piece.getSide()) {
                moves.insert(attackPos);
            }
        }
    }
}