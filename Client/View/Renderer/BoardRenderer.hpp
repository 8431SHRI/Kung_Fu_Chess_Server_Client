#pragma once

#include <string>

#include "img.hpp"

/*
 * @class BoardRenderer
 * @brief Responsible for drawing the game board background.
 * Purpose: Loads the board image once and copies it onto the rendering canvas.
 * Created by: Renderer | Used by: Renderer before drawing pieces.
 */

class BoardRenderer
{
private:

    Img boardImage;

public:

    explicit BoardRenderer(
        const std::string& boardPath);

    void drawBoard(
        Img& canvas);
};