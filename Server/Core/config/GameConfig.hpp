#pragma once
#include <unordered_map>
#include <string>
#include "Piece.hpp"
class GameConfig
{
public:

    static int BOARD_ROWS;

    static int BOARD_COLS;

    static int CELL_SIZE_PX;

    static int DEFAULT_TRAVEL_TIME_MS;

    static int INITIAL_TIME_MS;

    static int BOARD_WIDTH_PX;

    static int BOARD_HEIGHT_PX;

    static std::unordered_map<char, PieceType> TYPE_MAP;
    static char SIDE_WHITE;
    static char SIDE_BLACK;
    static constexpr int SHORT_REST_TIME_MS = 500;

    static constexpr int LONG_REST_TIME_MS = 1000;
};