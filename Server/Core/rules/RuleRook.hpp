#pragma once
#include "IMoveRule.hpp"
#include <set>

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;
class Position;

/**
 * @class RuleRook
 * @brief מימוש חוקי התנועה של הצריח (Rook).
 * יורש מ-IMoveRule ומגדיר את הלוגיקה הספציפית לצריח, המאפשרת לו לנוע בקווים ישרים
 * (אופקית ואנכית) ללא הגבלת מרחק כל עוד המסלול פנוי.
 */
class RuleRook : public IMoveRule {
public:
    /**
     * @brief מחזיר את כל מיקומי היעד החוקיים עבור הצריח בהתאם למצב הלוח הנוכחי.
     * @param board לוח המשחק.
     * @param piece הצריח שעבורו בודקים את המהלכים.
     * @return std::set<Position> קבוצת המיקומים אליהם הצריח רשאי לנוע.
     */
    std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const override;
};