#pragma once

#include "Piece.hpp"

#include <memory>

class StateTimer
{
private:

    std::shared_ptr<Piece> piece;

    PieceState currentState;

    PieceState nextState;

    int startTimeMs;

    int durationMs;

public:

    StateTimer(
        std::shared_ptr<Piece> piece,
        PieceState currentState,
        PieceState nextState,
        int startTime,
        int duration);

    bool hasFinished(int currentTime) const;

    double getProgress(int currentTime) const;

    std::shared_ptr<Piece> getPiece() const;

    PieceState getCurrentState() const;

    PieceState getNextState() const;

    int getStartTime() const;

    int getDuration() const;
};