#include "BoardRenderer.hpp"

#include "GameConfig.hpp"
#include "OpenCVConfig.hpp"

#include <opencv2/imgproc.hpp>

BoardRenderer::BoardRenderer(
    const std::string& boardPath)
{
    boardImage.read(
        boardPath,
        {
            GameConfig::BOARD_WIDTH_PX,
            GameConfig::BOARD_HEIGHT_PX
        },
        false);

    if (boardImage.get_mat().channels() ==
        OpenCVConfig::RGB_CHANNELS)
    {
        cv::cvtColor(
            boardImage.get_mat(),
            boardImage.get_mat(),
            cv::COLOR_BGR2BGRA);
    }
}

void BoardRenderer::drawBoard(
    Img& canvas)
{
    boardImage.get_mat().copyTo(
        canvas.get_mat());
}