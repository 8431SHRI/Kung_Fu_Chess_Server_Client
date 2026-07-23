#pragma once

#include "SelectionModel.hpp"
#include "img.hpp"

/*
 * @class SelectionRenderer
 * @brief Draws the selected square and legal move indicators.
 * Purpose: Visual feedback for user selection.
 * Created by: Renderer | Used by: Renderer.
 */

class SelectionRenderer
{
public:

    void draw(
        const SelectionModel& selection,
        Img& canvas);
};