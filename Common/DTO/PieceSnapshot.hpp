#pragma once

#include "Piece.hpp"
#include "Position.hpp"
/*
 * @struct PieceSnapshot
 * @brief Represents a snapshot of an individual piece for screen rendering.
 * Purpose: Stores graphical data of a specific piece without direct access to the Piece model object.
 * Contains: Piece identity, type and color, animation state, board position, and motion data for interpolation.
 * Created by: GameEngine::snapshot() | Used by: PieceRenderer.
 */
struct PieceSnapshot
{
    int id;

    Side side;

    PieceType type;

    PieceState state;

    Position boardPosition;

    bool isMoving = false;

    Position motionSource;
    
    Position motionDestination;

    double progress = 0.0;
};
