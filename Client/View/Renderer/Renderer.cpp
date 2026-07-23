#include "Renderer.hpp"

#include "GameConfig.hpp"
#include "GuiConfig.hpp"

#include <opencv2/imgproc.hpp>

Renderer::Renderer(
    ResourceManager& resources,
    const std::string& boardPath)
    :
    boardRenderer(boardPath),
    pieceRenderer(resources)
{
}

void Renderer::render(
    const GameSnapshot& snapshot,
    const SelectionModel& selection,
    Img& canvas,
    int elapsedMs)
{
    boardRenderer.drawBoard(canvas);

    selectionRenderer.draw(
        selection,
        canvas);

    for (const auto& piece : snapshot.pieces)
    {
        pieceRenderer.drawPiece(
            piece,
            canvas,
            elapsedMs);
    }

    if (snapshot.gameOver)
    {
        cv::rectangle(
            canvas.get_mat(),
            cv::Rect(
                0,
                0,
                GameConfig::BOARD_WIDTH_PX,
                GameConfig::BOARD_HEIGHT_PX),
            GuiConfig::GAME_OVER_OVERLAY_COLOR,
            -1);

        canvas.put_text(
            GuiConfig::GAME_OVER_TEXT,
            GuiConfig::GAME_OVER_TEXT_X,
            GuiConfig::GAME_OVER_TEXT_Y,
            GuiConfig::GAME_OVER_FONT_SCALE,
            GuiConfig::GAME_OVER_TEXT_COLOR,
            GuiConfig::GAME_OVER_FONT_THICKNESS);
    }
}