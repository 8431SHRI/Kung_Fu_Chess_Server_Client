#include "Jump.hpp"

Jump::Jump(std::shared_ptr<Piece> piece,
           Position position,
           int startTime,
           int duration)
    : jumpingPiece(piece),
      jumpPosition(position),
      startTimeMs(startTime),
      durationMs(duration)
{
}

bool Jump::hasFinished(int currentTime) const
{
    return currentTime >= startTimeMs + durationMs;
}

std::shared_ptr<Piece> Jump::getPiece() const
{
    return jumpingPiece;
}

const Position& Jump::getPosition() const
{
    return jumpPosition;
}

int Jump::getStartTime() const
{
    return startTimeMs;
}

int Jump::getDuration() const
{
    return durationMs;
}