#pragma once

#include <vector>
#include "PieceSnapshot.hpp"
/*
 * @struct GameSnapshot
 * @brief Represents a complete snapshot of the game state at a given moment.
 * Purpose: Used to pass necessary information to the Renderer without granting direct access to the Game Engine.
 * Contains: Board size, list of pieces and their states, game-over status, selected piece, and its legal moves.
 * Created by: GameEngine::snapshot() | Used by: Renderer, PieceRenderer, SelectionRenderer.
 */
struct GameSnapshot
{
    int rows;

    int cols;

    bool gameOver;

    std::vector<PieceSnapshot> pieces;

    bool hasSelection;

    Position selectedCell;

    std::vector<Position> legalMoves;
};