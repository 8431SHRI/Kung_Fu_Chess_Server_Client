#include "BoardFileConfig.hpp"

char BoardFileConfig::EMPTY_CELL='.';
char BoardFileConfig::SIDE_WHITE='w';
char BoardFileConfig::SIDE_BLACK='b';

std::unordered_map<char, PieceType>
BoardFileConfig::TYPE_MAP =
{
    {'K',PieceType::KING},
    {'Q',PieceType::QUEEN},
    {'R',PieceType::ROOK},
    {'B',PieceType::BISHOP},
    {'N',PieceType::KNIGHT},
    {'P',PieceType::PAWN}
};