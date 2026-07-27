#pragma once

#include <memory>

#include "IPlayerSource.hpp"
#include "Piece.hpp"
#include "RuleEngine.hpp"
#include "Board.hpp"
#include "IBotStrategy.hpp"
#include "GreedyCaptureBotStrategy.hpp"

/*
 * @class BotPlayerSource
 * @brief מימוש IPlayerSource שמייצר מהלכים אוטומטית עבור הצד שהוא מייצג.
 * Purpose: fallback ל-matchmaking כשאין יריב אנושי, ומצב "שחק נגד מחשב".
 * Note: האלגוריתם עצמו (איזה מהלך לבחור) גר עכשיו ב-Server/Bot (IBotStrategy) -
 *       BotPlayerSource הוא רק "adapter" ל-IPlayerSource, לא מחזיק לוגיקת החלטה בעצמו.
 *       ברירת המחדל (GreedyCaptureBotStrategy) שומרת תאימות מלאה להתנהגות הקודמת
 *       (מהלך חוקי רנדומלי) כשאין אכילה זמינה - אז הבדיקות הקיימות ממשיכות לעבוד.
 *       קפיצה (jump) עדיין נדירה: הבוט קופץ רק כ-fallback אחרון, כשאין לו אף מהלך
 *       חוקי זמין לאף כלי (כל הכלים שלו בתנועה/מנוחה) - "עדיף לקפוץ מאשר לא לעשות כלום".
 */
class BotPlayerSource : public IPlayerSource
{
public:
    BotPlayerSource(
        Side side,
        const RuleEngine& ruleEngine,
        const Board& board,
        std::shared_ptr<IBotStrategy> strategy = std::make_shared<GreedyCaptureBotStrategy>());

    bool hasPendingMove() const override;

    void requestMove(Position& outFrom, Position& outTo) override;

    bool hasPendingJump() const override;

    void requestJump(Position& outPosition) override;

private:
    Side side_;

    const RuleEngine& ruleEngine_;

    const Board& board_;

    std::shared_ptr<IBotStrategy> strategy_;

    bool tryPickJumpableIdlePiece(Position& outPosition) const;
};