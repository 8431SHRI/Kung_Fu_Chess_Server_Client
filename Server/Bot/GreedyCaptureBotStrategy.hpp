#pragma once

#include "IBotStrategy.hpp"

/*
 * @class GreedyCaptureBotStrategy
 * @brief משדרג את הבוט המקורי (שבחר מהלך חוקי לגמרי אקראי): אם קיים מהלך שאוכל
 *        כלי יריב - משחק אותו. אחרת, נופל חזרה לאותה בחירה אקראית כמו קודם.
 * Purpose: בוט v1 "משחק להזיז", לא באמת "יודע לשחק שחמט" - זו התקדמות קטנה,
 *          מספקת בהרבה, בלי להיכנס למינימקס/הערכת לוח בשלב הזה.
 * Note: אין העדפה בין כמה אכילות אפשריות - הראשונה שנמצאת (אחרי shuffle) נבחרת.
 */
class GreedyCaptureBotStrategy : public IBotStrategy
{
public:
    bool tryDecideMove(
        Side side,
        const Board& board,
        const RuleEngine& ruleEngine,
        Position& outFrom,
        Position& outTo) const override;
};
