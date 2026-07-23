#pragma once

#include <unordered_map>

#include "Piece.hpp"

class BoardFileConfig
{
public:

    static char EMPTY_CELL;

    static char SIDE_WHITE;

    static char SIDE_BLACK;

    static std::unordered_map<char, PieceType> TYPE_MAP;
};