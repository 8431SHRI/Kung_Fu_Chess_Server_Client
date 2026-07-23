#include "StateTimer.hpp"

StateTimer::StateTimer(
    std::shared_ptr<Piece> piece,
    PieceState currentState,
    PieceState nextState,
    int startTime,
    int duration)
    :
    piece(piece),
    currentState(currentState),
    nextState(nextState),
    startTimeMs(startTime),
    durationMs(duration)
{
}

bool StateTimer::hasFinished(int currentTime) const
{
    return currentTime >= startTimeMs + durationMs;
}

double StateTimer::getProgress(int currentTime) const
{
    if (currentTime <= startTimeMs)
        return 0.0;

    if (currentTime >= startTimeMs + durationMs)
        return 1.0;

    return static_cast<double>(
               currentTime - startTimeMs)
           /
           static_cast<double>(durationMs);
}

std::shared_ptr<Piece> StateTimer::getPiece() const
{
    return piece;
}

PieceState StateTimer::getCurrentState() const
{
    return currentState;
}

PieceState StateTimer::getNextState() const
{
    return nextState;
}

int StateTimer::getStartTime() const
{
    return startTimeMs;
}

int StateTimer::getDuration() const
{
    return durationMs;
}