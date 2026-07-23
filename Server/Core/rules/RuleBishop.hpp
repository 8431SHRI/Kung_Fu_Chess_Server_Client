#pragma once
#include "IMoveRule.hpp"
#include <set>

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;
class Position;

/**
 * @class RuleBishop
 * @brief מימוש חוקי התנועה של הרץ (Bishop).
 * יורש מ-IMoveRule ומגדיר את הלוגיקה הספציפית לרץ, המאפשרת לו לנוע באלכסונים ללא הגבלת מרחק (כל עוד המסלול פנוי).
 */
class RuleBishop : public IMoveRule {
public:
    /**
     * @brief מחזיר את כל מיקומי היעד החוקיים עבור הרץ בהתאם למצב הלוח הנוכחי.
     * @param board לוח המשחק.
     * @param piece הרץ שעבורו בודקים את המהלכים.
     * @return std::set<Position> קבוצת המיקומים אליהם הרץ רשאי לנוע.
     */
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;
};