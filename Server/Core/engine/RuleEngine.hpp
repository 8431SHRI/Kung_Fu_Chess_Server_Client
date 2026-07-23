#pragma once
#include <map>
#include <memory>
#include <string>
#include "Board.hpp"
#include "IMoveRule.hpp"
#include <set>

// מבנה נתונים לייצוג תוצאת אימות תנועה
struct MoveValidation {
    bool is_valid;
    std::string reason;
};

/**
 * @class RuleEngine
 * @brief מנוע חוקי המשחק.
 * אחראי על ריכוז וניהול חוקי התנועה של כל סוגי הכלים ואימות תקינות המהלכים על הלוח.
 */
class RuleEngine {
private:
    // מפה דינמית שממפה בין סוג כלי לבין חוק התנועה הייחודי שלו (Strategy Pattern)
    std::map<PieceType, std::unique_ptr<IMoveRule>> moveRules;

public:
    /**
     * @brief בנאי המאתחל את מפת החוקים עם אסטרטגיות התנועה השונות.
     */
    RuleEngine();

    /**
     * @brief בודק האם תנועה של כלי ממיקום מסוים למיקום אחר היא חוקית.
     * * @param to מיקום היעד המבוקש.
     * @param from מיקום המקור הנוכחי של הכלי.
     * @param grid לוח המשחק המשמש לבדיקת הכלים והמשבצות.
     * @return MoveValidation מבנה המכיל את סטטוס הבדיקה והסבר במידה והתנועה נכשלה.
     */
    MoveValidation isValidMove(const Position& to, const Position& from, const Board& grid);

    /**
     * @brief בודק האם המצב הפנימי הנוכחי של הכלי מאפשר לו לבצע פעולה/קפיצה.
     * * @param piece מצביע חכם לכלי הנבדק.
     * @return MoveValidation מבנה המכיל את סטטוס הבדיקה.
     */
    MoveValidation isValidJump(std::shared_ptr<Piece> piece) const;
    std::set<Position> getLegalMoves(
    const Position& from,
    const Board& board);
};