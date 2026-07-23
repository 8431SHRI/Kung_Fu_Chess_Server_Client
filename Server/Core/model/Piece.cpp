#include "Piece.hpp"
#include <stdexcept>
std::string Piece::toCanonicalString() const
{
    std::string sideChar =
        (side == Side::WHITE) ? "w" : "b";

    return sideChar +
           getTypeFolderName(type);
}

std::string Piece::getTypeFolderName(PieceType type)
{
    switch (type)
    {
    case PieceType::KING:
        return "king";

    case PieceType::QUEEN:
        return "queen";

    case PieceType::ROOK:
        return "rook";

    case PieceType::BISHOP:
        return "bishop";

    case PieceType::KNIGHT:
        return "knight";

    case PieceType::PAWN:
        return "pawn";
    }

    throw std::runtime_error("Unknown PieceType");
}

std::string Piece::getSideFolderName(Side side)
{
    switch (side)
    {
    case Side::WHITE:
        return "white";

    case Side::BLACK:
        return "black";
    }

    throw std::runtime_error("Unknown Side");
}

std::string Piece::getStateFolderName(PieceState state)
{
    switch (state)
    {
    case PieceState::IDLE:
        return "idle";

    case PieceState::MOVING:
        return "move";

    case PieceState::AIRBORNE:
        return "jump";

    case PieceState::CAPTURED:
        return "captured";
    case PieceState::SHORT_REST:
        return "short_rest";

    case PieceState::LONG_REST:
        return "long_rest";
    }

    throw std::runtime_error("Unknown PieceState");
}