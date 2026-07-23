#pragma once

#include "Position.hpp"

/*
 * @interface IPlayerSource
 * @brief הפשטה למקור מהלכים עבור צד אחד במשחק — יכול להיות שחקן אנושי (רשת) או בוט.
 * Purpose: מאפשר ל-GameSession/Scheduler לא להבדיל בין אדם לבוט.
 * Implemented by: NetworkPlayerSource, BotPlayerSource.
 */
class IPlayerSource
{
public:
    virtual ~IPlayerSource() = default;

    // נקרא ע"י GameSession בכל tick כדי לבדוק אם יש פקודה חדשה ממתינה.
    // מימוש חייב להיות לא-חוסם (non-blocking) — GameSession/Scheduler הם single-threaded.
    virtual bool hasPendingMove() const = 0;

    virtual void requestMove(Position& outFrom, Position& outTo) = 0;

    virtual bool hasPendingJump() const = 0;

    virtual void requestJump(Position& outPosition) = 0;
};