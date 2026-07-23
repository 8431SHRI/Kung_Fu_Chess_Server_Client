#pragma once

#include <string>

#include "BoardRenderer.hpp"
#include "PieceRenderer.hpp"
#include "ResourceManager.hpp"
#include "GameSnapshot.hpp"
#include "SelectionRenderer.hpp"
#include "SelectionModel.hpp"

/*
 * @class Renderer
 * @brief Responsible for drawing the entire game screen.
 * Purpose: Draws the board, selection markers, pieces, and GAME OVER overlay.
 * Created by: main() | Used by: Window.
 */

class Renderer
{
private:

    BoardRenderer boardRenderer;

    SelectionRenderer selectionRenderer;

    PieceRenderer pieceRenderer;

public:

    Renderer(
        ResourceManager& resources,
        const std::string& boardPath);

    void render(
        const GameSnapshot& snapshot,
        const SelectionModel& selection,
        Img& canvas,
        int elapsedMs);
};