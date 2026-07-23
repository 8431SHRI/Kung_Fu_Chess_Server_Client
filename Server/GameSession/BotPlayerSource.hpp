#pragma once

#include "IPlayerSource.hpp"
#include "Piece.hpp"
#include "RuleEngine.hpp"
#include "Board.hpp"

/*
 * @class BotPlayerSource
 * @brief מימוש IPlayerSource שמייצר מהלכים אוטומטית עבור הצד שהוא מייצג.
 * Purpose: fallback ל-matchmaking כשאין יריב אנושי, ומצב "שחק נגד מחשב".
 * Note: קורא ל-RuleEngine::getLegalMoves הקיים בלבד — לא מוסיף/משנה לוגיקת חוקיות.
 *       הבוט הזה תמיד מזיז (לא קופץ) — קפיצה היא יכולת נדירה יותר, לא הכרחית ל-v1.
 */
class BotPlayerSource : public IPlayerSource
{
public:
    BotPlayerSource(Side side, const RuleEngine& ruleEngine, const Board& board);

    bool hasPendingMove() const override;

    void requestMove(Position& outFrom, Position& outTo) override;

    bool hasPendingJump() const override;

    void requestJump(Position& outPosition) override;

private:
    Side side_;

    const RuleEngine& ruleEngine_;

    const Board& board_;

    bool tryPickRandomLegalMove(Position& outFrom, Position& outTo) const;
};