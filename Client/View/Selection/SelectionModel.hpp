#pragma once

#include <optional>
#include <set>
#include <vector>

#include "Position.hpp"
/*
 * @class SelectionModel
 * @brief Maintains the user's current selection state.
 * Purpose: Manages the selected piece and its legal moves for display purposes.
 * Created by: main() | Used by: GameController (updating), SelectionRenderer/Window (reading).
 * Functions: clear() (reset), select() (new selection), hasSelection/getSelection() (query), set/getLegalMoves() (moves management).
 */
class SelectionModel
{
private:
    std::optional<Position> selectedSquare;

    std::set<Position> legalMoves;

public:
    SelectionModel() = default;

    void clear();

    void select(const Position &pos);

    bool hasSelection() const;

    const std::optional<Position> &getSelection() const;

    void setLegalMoves(const std::set<Position> &moves);

    const std::set<Position> &getLegalMoves() const;
};