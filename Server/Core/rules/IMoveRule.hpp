#pragma once
#include <set>
#include "Board.hpp"
#include "Piece.hpp"

/**
 * @brief ממשק בסיס למערכת חוקי התנועה.
 * כל סוג כלי יממש מחלקה שיורשת מממשק זה כדי להגדיר את המהלכים החוקיים שלו.
 */
class IMoveRule {
public:
    virtual ~IMoveRule() = default;

    /**
     * @brief מחשב את כל התאים החוקיים אליהם הכלי יכול לעבור.
     * @param board הלוח הנוכחי לבדיקת מכשולים וגבולות.
     * @param piece הכלי שעבורו מחושבים המהלכים.
     * @return קבוצת מיקומים חוקיים.
     */
    virtual std::set<Position> getLegalDestinations(const Board& board, const Piece& piece) const = 0;
};