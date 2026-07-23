#pragma once
#include "IMoveRule.hpp"
#include <set>

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;
class Position;

/**
 * @class RuleKnight
 * @brief מימוש חוקי התנועה של הפרש (Knight).
 * יורש מ-IMoveRule ומגדיר את הלוגיקה הספציפית לפרש, המאפשרת לו לקפוץ בצורת L לכל כיוון.
 */
class RuleKnight : public IMoveRule {
public:
    /**
     * @brief מחזיר את כל מיקומי היעד החוקיים עבור הפרש בהתאם למצב הלוח הנוכחי.
     * @param board לוח המשחק.
     * @param piece הפרש שעבורו בודקים את המהלכים.
     * @return std::set<Position> קבוצת המיקומים אליהם הפרש רשאי לנוע.
     */
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;
};