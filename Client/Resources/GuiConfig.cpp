#include "GuiConfig.hpp"

int GuiConfig::SELECTION_BORDER_THICKNESS = 3;
int GuiConfig::LEGAL_MOVE_BORDER_THICKNESS = 2;
double GuiConfig::GAME_OVER_FONT_SCALE = 3.5;
int GuiConfig::GAME_OVER_FONT_THICKNESS = 6;

cv::Scalar GuiConfig::SELECTION_COLOR = cv::Scalar(0,255,255,255);
cv::Scalar GuiConfig::LEGAL_MOVE_COLOR = cv::Scalar(0,220,0,255);
cv::Scalar GuiConfig::GAME_OVER_OVERLAY_COLOR = cv::Scalar(0,0,0,170);
cv::Scalar GuiConfig::GAME_OVER_TEXT_COLOR = cv::Scalar(255,255,255,255);

const char* GuiConfig::GAME_OVER_TEXT = "GAME OVER";
int GuiConfig::GAME_OVER_TEXT_X = 120;
int GuiConfig::GAME_OVER_TEXT_Y = 420;