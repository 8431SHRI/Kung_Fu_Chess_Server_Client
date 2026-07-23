#pragma once
#include "IMoveRule.hpp"
#include <set>

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;
class Position;

/**
 * @class RuleQueen
 * @brief מימוש חוקי התנועה של המלכה (Queen).
 * יורש מ-IMoveRule ומגדיר את הלוגיקה הספציפית למלכה, המשלבת את כיווני התנועה של הצריח והרץ
 * (אופקית, אנכית ואלכסונית) ללא הגבלת מרחק כל עוד המסלול פנוי.
 */
class RuleQueen : public IMoveRule {
public:
    /**
     * @brief מחזיר את כל מיקומי היעד החוקיים עבור המלכה בהתאם למצב הלוח הנוכחי.
     * @param board לוח המשחק.
     * @param piece המלכה שעבורה בודקים את המהלכים.
     * @return std::set<Position> קבוצת המיקומים אליהם המלכה רשאית לנוע.
     */
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;
};