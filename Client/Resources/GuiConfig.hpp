#pragma once
#include <opencv2/opencv.hpp>

class GuiConfig
{
public:
    static int SELECTION_BORDER_THICKNESS;
    static int LEGAL_MOVE_BORDER_THICKNESS;

    static double GAME_OVER_FONT_SCALE;
    static int GAME_OVER_FONT_THICKNESS;

    static cv::Scalar SELECTION_COLOR;
    static cv::Scalar LEGAL_MOVE_COLOR;
    static cv::Scalar GAME_OVER_OVERLAY_COLOR;
    static cv::Scalar GAME_OVER_TEXT_COLOR;

    static const char *GAME_OVER_TEXT;
    static int GAME_OVER_TEXT_X;
    static int GAME_OVER_TEXT_Y;
};