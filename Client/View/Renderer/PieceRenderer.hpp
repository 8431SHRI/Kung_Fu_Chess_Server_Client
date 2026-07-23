#pragma once

#include "Animation.hpp"
#include "PieceSnapshot.hpp"
#include "ResourceManager.hpp"
#include "img.hpp"

/*
 * @class PieceRenderer
 * @brief Draws a single game piece including movement animation.
 * Purpose: Selects the correct animation and computes the piece position.
 * Created by: Renderer | Used by: Renderer.
 */

class PieceRenderer
{
private:

    ResourceManager& resources;

public:

    explicit PieceRenderer(
        ResourceManager& resourceManager);

    void drawPiece(
        const PieceSnapshot& piece,
        Img& canvas,
        int elapsedMs);
};