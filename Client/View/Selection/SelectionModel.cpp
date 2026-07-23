#include "SelectionModel.hpp"

void SelectionModel::clear()
{
    selectedSquare.reset();
    legalMoves.clear();
}

void SelectionModel::select(const Position& pos)
{
    selectedSquare = pos;
}

bool SelectionModel::hasSelection() const
{
    return selectedSquare.has_value();
}

const std::optional<Position>&
SelectionModel::getSelection() const
{
    return selectedSquare;
}

void SelectionModel::setLegalMoves(
    const std::set<Position>& moves)
{
    legalMoves = moves;
}

const std::set<Position>&
SelectionModel::getLegalMoves() const
{
    return legalMoves;
}
