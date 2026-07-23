#pragma once

#include <memory>
#include "Piece.hpp"
#include "Position.hpp"

/**
 * @class Jump
 * @brief מייצג קפיצה פעילה של כלי.
 *
 * האובייקט אינו מזיז את הכלי ואינו משנה את מצב הלוח.
 * תפקידו היחיד הוא לשמור את נתוני הקפיצה
 * ולאפשר ל־RealTimeArbiter לבדוק מתי הקפיצה מסתיימת.
 */
class Jump
{
private:

    /// הכלי שקופץ
    std::shared_ptr<Piece> jumpingPiece;

    /// התא שעליו הכלי נשאר בזמן הקפיצה
    Position jumpPosition;

    /// זמן תחילת הקפיצה
    int startTimeMs;

    /// משך הקפיצה
    int durationMs;

public:

    /**
     * @brief יוצר אובייקט Jump חדש.
     *
     * @param piece הכלי שקופץ.
     * @param position התא שעליו הכלי נמצא.
     * @param startTime זמן תחילת הקפיצה.
     * @param duration משך הקפיצה.
     */
    Jump(std::shared_ptr<Piece> piece,
         Position position,
         int startTime,
         int duration);

    /**
     * @brief בודק האם הקפיצה הסתיימה.
     */
    bool hasFinished(int currentTime) const;

    /**
     * @brief מחזיר את הכלי שקופץ.
     */
    std::shared_ptr<Piece> getPiece() const;

    /**
     * @brief מחזיר את התא שעליו מתבצעת הקפיצה.
     */
    const Position& getPosition() const;

    /**
     * @brief מחזיר את זמן תחילת הקפיצה.
     */
    int getStartTime() const;

    /**
     * @brief מחזיר את משך הקפיצה.
     */
    int getDuration() const;
};