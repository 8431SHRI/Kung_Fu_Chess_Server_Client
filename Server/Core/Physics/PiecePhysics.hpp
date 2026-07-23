#pragma once

#include "Piece.hpp"

struct PiecePhysics
{
    double speedCellsPerSecond = 0.0;

    PieceState nextState = PieceState::IDLE;

    int restTimeMs = 0;
};