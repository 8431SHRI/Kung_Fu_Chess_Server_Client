#pragma once
#include <vector>
#include <memory>
#include "Piece.hpp"
#include "Position.hpp"

/**
 * @class Board
 * @brief מייצג את לוח המשחק.
 * מנהל את המטריצה הדו-ממדית (גריד) של הכלים בלוח, ומאפשר גישה, הזזה והסרה שלהם בצורה בטוחה.
 */
class Board {
private:
    int rows, cols;
    std::vector<std::vector<std::shared_ptr<Piece>>> grid;

public:
    /**
     * @brief בנאי המאתחל את הלוח עם מספר שורות ועמודות מוגדר, וממלא אותו ב-nullptr.
     * @param r מספר השורות בלוח.
     * @param c מספר העמודות בלוח.
     */
    Board(int r, int c);
    
    /**
     * @brief מחזיר את מספר השורות בלוח.
     */
    int getRows() const;
    
    /**
     * @brief מחזיר את מספר העמודות בלוח.
     */
    int getCols() const;
    
    /**
     * @brief מחזיר את הכלי הנמצא במיקום מסוים בלוח (אם קיים ובתוך הגבולות).
     * @param pos המיקום המבוקש לבדיקה.
     * @return std::shared_ptr<Piece> מצביע לכלי המבוקש, או nullptr אם המשבצת ריקה או מחוץ ללוח.
     */
    std::shared_ptr<Piece> getPieceAt(const Position& pos) const;
    
    /**
     * @brief מציב כלי במיקום ספציפי בלוח ומעדכן את קואורדינטות המיקום הפנימיות של הכלי עצמו.
     * @param r מספר שורה.
     * @param c מספר עמודה.
     * @param p מצביע לכלי שיש להציב.
     */
    void setPieceAt(int r, int c, std::shared_ptr<Piece> p);
    
    /**
     * @brief בודק האם מיקום מסוים נמצא בתוך גבולות הלוח.
     * @param to המיקום לבדיקה.
     * @return true אם המיקום חוקי ובתוך גבולות הלוח, false אחרת.
     */
    bool isInsideBoard(const Position& to) const;
    
    /**
     * @brief מעביר כלי ממיקום אחד לאחר על גבי הלוח.
     * @param from המיקום הנוכחי ממנו מעבירים את הכלי.
     * @param to מיקום היעד המבוקש.
     */
    void movePiece(const Position& from, const Position& to);

    /**
     * @brief מסירה כלי ממיקום ספציפי בלוח (הופך את המשבצת ל-nullptr).
     * @param pos המיקום ממנו רוצים להסיר את הכלי.
     */
    void removePiece(const Position& pos);
};