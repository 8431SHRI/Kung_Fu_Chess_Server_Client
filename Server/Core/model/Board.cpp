#include "Board.hpp"

// מימוש הבנאי - מקצה מטריצה בגודל r x c ומאתחל אותה ב-nullptr
Board::Board(int r, int c) 
    : rows(r), cols(c), grid(r, std::vector<std::shared_ptr<Piece>>(c, nullptr)) {}

int Board::getRows() const { 
    return rows; 
}

int Board::getCols() const { 
    return cols; 
}

std::shared_ptr<Piece> Board::getPieceAt(const Position& pos) const {
    // בדיקה בטוחה שהקואורדינטות בטווח לפני גישה ישירה למערך
    return (pos.getRow() >= 0 && pos.getRow() < rows && pos.getCol() >= 0 && pos.getCol() < cols) 
           ? grid[pos.getRow()][pos.getCol()] : nullptr;
}

void Board::setPieceAt(int r, int c, std::shared_ptr<Piece> p) {
    Position pos(r, c);
    if (isInsideBoard(pos)) {
        grid[r][c] = p;
        if (p) {
            p->setPosition(pos); // עדכון המיקום הפנימי של האובייקט
        }
    }
}

bool Board::isInsideBoard(const Position& to) const {
    return to.getRow() < rows && to.getRow() >= 0 && to.getCol() < cols && to.getCol() >= 0;
}

void Board::movePiece(const Position& from, const Position& to) {
    if (!isInsideBoard(from) || !isInsideBoard(to)) return;

    // עצירה מוקדמת: אם המקור והיעד זהים, אין מה לבצע
    if (from.getRow() == to.getRow() && from.getCol() == to.getCol()) return;

    std::shared_ptr<Piece> piece = grid[from.getRow()][from.getCol()];
    
    if (piece) {
        // 1. הצבת הכלי ביעד (setPieceAt דואגת לעדכן את המערך ואת הכלי עצמו)
        setPieceAt(to.getRow(), to.getCol(), piece);
        
        // 2. ניקוי המיקום הישן בלוח 
        grid[from.getRow()][from.getCol()] = nullptr; 
    }
}

void Board::removePiece(const Position& pos) {
    if (isInsideBoard(pos)) {
        grid[pos.getRow()][pos.getCol()] = nullptr;
    }
}