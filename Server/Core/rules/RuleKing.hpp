#pragma once
#include "IMoveRule.hpp"
#include <set>

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;
class Position;

/**
 * @class RuleKing
 * @brief מימוש חוקי התנועה של המלך (King).
 * יורש מ-IMoveRule ומגדיר את הלוגיקה הספציפית למלך, המאפשרת לו לנוע משבצת אחת לכל כיוון (אנכית, אופקית או אלכסונית).
 */
class RuleKing : public IMoveRule {
public:
    /**
     * @brief מחזיר את כל מיקומי היעד החוקיים עבור המלך בהתאם למצב הלוח הנוכחי.
     * @param board לוח המשחק.
     * @param piece המלך שעבורו בודקים את המהלכים.
     * @return std::set<Position> קבוצת המיקומים אליהם המלך רשאי לנוע.
     */
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;
};