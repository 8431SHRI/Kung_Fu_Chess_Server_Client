#pragma once

#include "Board.hpp"
#include "Position.hpp"
#include "RuleEngine.hpp"

/*
 * @interface IBotStrategy
 * @brief מפריד בין "מתי/איך לשאול את הבוט" (BotPlayerSource, ב-Server/GameSession)
 *        לבין "איך הבוט מחליט מה לשחק" (המימושים כאן, ב-Server/Bot).
 * Purpose: מאפשר להחליף אלגוריתם (רנדומלי / מעדיף אכילות / מינימקס וכו') בלי לגעת
 *          ב-BotPlayerSource או ב-IPlayerSource בכלל.
 */
class IBotStrategy
{
public:
    virtual ~IBotStrategy() = default;

    // מנסה לבחור מהלך (from -> to) עבור side. מחזיר false אם אין אף מהלך חוקי כרגע.
    virtual bool tryDecideMove(
        Side side,
        const Board& board,
        const RuleEngine& ruleEngine,
        Position& outFrom,
        Position& outTo) const = 0;
};
