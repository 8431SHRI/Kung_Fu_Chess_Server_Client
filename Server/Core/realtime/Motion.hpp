#pragma once

#include "Board.hpp"
#include "Piece.hpp"

#include <memory>

class Motion
{
public:
    std::shared_ptr<Piece> movingPiece;

    Position source;
    Position destination;

    int startTime;
    int finishTime;
    PieceState nextState;

    Motion(
        std::shared_ptr<Piece> piece,
        Position src,
        Position dst,
        int start,
        int travelTime,
        PieceState next)
        : movingPiece(piece),
          source(src),
          destination(dst),
          startTime(start),
          finishTime(start + travelTime),
          nextState(next)
    {
    }
    

    bool hasArrived(int currentTime) const
    {
        return currentTime >= finishTime;
    }

    double getProgress(int currentTime) const
    {
        if (currentTime <= startTime)
            return 0.0;

        if (currentTime >= finishTime)
            return 1.0;

        return static_cast<double>(currentTime - startTime) /
               static_cast<double>(finishTime - startTime);
    }
};