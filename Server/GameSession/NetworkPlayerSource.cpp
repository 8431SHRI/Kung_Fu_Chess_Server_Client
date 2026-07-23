#include "NetworkPlayerSource.hpp"

void NetworkPlayerSource::enqueueMove(const Position& from, const Position& to)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pendingMove_ = PendingMove{from, to};
}

void NetworkPlayerSource::enqueueJump(const Position& position)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pendingJump_ = position;
}

bool NetworkPlayerSource::hasPendingMove() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pendingMove_.has_value();
}

void NetworkPlayerSource::requestMove(Position& outFrom, Position& outTo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    outFrom = pendingMove_->from;
    outTo = pendingMove_->to;
    pendingMove_.reset();
}

bool NetworkPlayerSource::hasPendingJump() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pendingJump_.has_value();
}

void NetworkPlayerSource::requestJump(Position& outPosition)
{
    std::lock_guard<std::mutex> lock(mutex_);
    outPosition = *pendingJump_;
    pendingJump_.reset();
}