#pragma once
#include "Piece.hpp"
#include "GameConfig.hpp"
#include <memory>
#include <string>
#include <atomic> // שומר על ה-ID ייחודי ובטוח גם בסביבה מרובת תהליכונים

/**
 * @class PieceEngine
 * @brief מנוע יצירת הכלים.
 * אחראי על הפיכת נתונים גולמיים לאובייקטים של כלים בצורה דינמית (DRY) והקצאת מזהים ייחודיים.
 */
class PieceEngine {
private:
    // מונה סטטי שמתחיל מ-1. מוגדר כאן ומוצהר בקובץ ה-CPP.
    static std::atomic<int> nextId;

public:
    /**
     * @brief יצירת כלי חדש על סמך סוג, צד ומיקום.
     * משתמש במפת הכלים הדינמית ומקצה מזהה (ID) ייחודי רציף ואוטומטי.
     */
    static std::shared_ptr<Piece> createPiece(char typeChar, Side side, Position pos);

    /**
     * @brief בדיקה האם סוג הכלי מוכר למנוע.
     */
    static bool isKnownType(char type);

    /**
     * @brief פונקציה שמקבלת טוקן גולמי (למשל "wK"), מאמתת אותו ומייצרת כלי עם ID אוטומטי.
     * @return std::shared_ptr<Piece> מצביע לכלי החדש, או nullptr אם הטוקן שגוי.
     */
    static std::shared_ptr<Piece> createFromToken(const std::string& token, const Position& pos);

    /**
     * @brief מאפס את מונה המזהים חזרה ל-1.
     * חובה לקרוא לפונקציה זו כאשר מאתחלים לוח חדש או מתחילים משחק מאפס!
     */
    static void resetIdCounter();
};