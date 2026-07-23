#include "GameConfig.hpp"

int GameConfig::BOARD_ROWS = 8;

int GameConfig::BOARD_COLS = 8;

int GameConfig::CELL_SIZE_PX = 100;

int GameConfig::DEFAULT_TRAVEL_TIME_MS = 1000;

int GameConfig::INITIAL_TIME_MS = 0;

int GameConfig::BOARD_WIDTH_PX =
    GameConfig::BOARD_COLS *
    GameConfig::CELL_SIZE_PX;

int GameConfig::BOARD_HEIGHT_PX =
    GameConfig::BOARD_ROWS *
    GameConfig::CELL_SIZE_PX;

char GameConfig::SIDE_WHITE = 'w';
char GameConfig::SIDE_BLACK = 'b';

std::unordered_map<char, PieceType> GameConfig::TYPE_MAP = {
    {'K', PieceType::KING},
    {'Q', PieceType::QUEEN},
    {'R', PieceType::ROOK},
    {'B', PieceType::BISHOP},
    {'N', PieceType::KNIGHT},
    {'P', PieceType::PAWN}
};