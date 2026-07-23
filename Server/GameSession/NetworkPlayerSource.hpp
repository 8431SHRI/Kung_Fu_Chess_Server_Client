#pragma once

#include <mutex>
#include <optional>

#include "IPlayerSource.hpp"

/*
 * @class NetworkPlayerSource
 * @brief מימוש IPlayerSource שמקבל פקודות מחיבור WebSocket ספציפי.
 * Purpose: הגשר בין ConnectionHandler (שמפענח MoveMsg/JumpMsg) לבין GameSession.
 * Note: enqueueMove/enqueueJump נקראים מ-thread הרשת; hasPendingMove/requestMove
 *       נקראים מ-thread ה-Scheduler. ה-mutex כאן קיים בדיוק בשביל זה.
 */
class NetworkPlayerSource : public IPlayerSource
{
public:
    NetworkPlayerSource() = default;

    // נקרא ע"י ConnectionHandler כשמגיעה הודעת MOVE/JUMP מהרשת (שלב Server/Network)
    void enqueueMove(const Position& from, const Position& to);

    void enqueueJump(const Position& position);

    bool hasPendingMove() const override;

    void requestMove(Position& outFrom, Position& outTo) override;

    bool hasPendingJump() const override;

    void requestJump(Position& outPosition) override;

private:
    mutable std::mutex mutex_;

    struct PendingMove { Position from; Position to; };

    std::optional<PendingMove> pendingMove_;

    std::optional<Position> pendingJump_;
};