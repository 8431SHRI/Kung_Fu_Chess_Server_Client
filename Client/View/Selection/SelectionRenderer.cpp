#include "SelectionRenderer.hpp"

#include "GameConfig.hpp"
#include "GuiConfig.hpp"

#include <opencv2/imgproc.hpp>

void SelectionRenderer::draw(
    const SelectionModel& selection,
    Img& canvas)
{
    cv::Mat& image =
        canvas.get_mat();

    if (selection.hasSelection())
    {
        const Position& pos =
            selection.getSelection().value();

        cv::rectangle(
            image,
            cv::Rect(
                pos.getCol() *
                GameConfig::CELL_SIZE_PX,

                pos.getRow() *
                GameConfig::CELL_SIZE_PX,

                GameConfig::CELL_SIZE_PX,

                GameConfig::CELL_SIZE_PX),

            GuiConfig::SELECTION_COLOR,

            GuiConfig::SELECTION_BORDER_THICKNESS);
    }

    for (const Position& pos :
         selection.getLegalMoves())
    {
        cv::rectangle(
            image,
            cv::Rect(
                pos.getCol() *
                GameConfig::CELL_SIZE_PX,

                pos.getRow() *
                GameConfig::CELL_SIZE_PX,

                GameConfig::CELL_SIZE_PX,

                GameConfig::CELL_SIZE_PX),

            GuiConfig::LEGAL_MOVE_COLOR,

            GuiConfig::LEGAL_MOVE_BORDER_THICKNESS);
    }
}