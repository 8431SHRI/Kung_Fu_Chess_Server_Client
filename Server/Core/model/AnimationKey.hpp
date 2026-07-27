#pragma once

#include "Piece.hpp"

struct AnimationKey
{
    PieceType type;

    Side side;

    PieceState state;

    int size;

    bool operator<(const AnimationKey& other) const
    {
        if (type != other.type)
            return type < other.type;

        if (side != other.side)
            return side < other.side;

        if (state != other.state)
            return state < other.state;

        return size < other.size;
    }
};