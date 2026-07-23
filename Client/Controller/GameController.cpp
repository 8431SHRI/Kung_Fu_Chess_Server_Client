#include "GameController.hpp"
#include <iostream>

GameController::GameController(
    Board &b,
    GameEngine &ge,
    SelectionModel &sm)
    : board(b),
      gameEngine(ge),
      selection(sm)
{
}

void GameController::jump(
    int pixelX,
    int pixelY)
{
    Position pos =
        BoardMapper::pixelToPosition(
            pixelX,
            pixelY);

    if (!board.isInsideBoard(pos))
        return;

    MoveResult result =
        gameEngine.requestJump(pos);

    if (!result.success)
    {
        std::cout
            << "ERROR "
            << result.reason
            << std::endl;
    }
}

void GameController::click(
    int pixelX,
    int pixelY)
{
    std::cout
        << "CLICK "
        << pixelX
        << " "
        << pixelY
        << std::endl;
    Position pos =
        BoardMapper::pixelToPosition(
            pixelX,
            pixelY);

    if (!board.isInsideBoard(pos))
        return;

    std::shared_ptr<Piece> clickedPiece =
        board.getPieceAt(pos);

    // אין כרגע כלי מסומן
    if (!selection.hasSelection())
    {
        if (clickedPiece != nullptr)
        {
            if (clickedPiece->getState() != PieceState::IDLE)
            {
                return;
            }

            selection.select(pos);

            selection.setLegalMoves(
                gameEngine.getLegalMoves(pos));
        }

        return;
    }

    Position selectedPos =
        selection.getSelection().value();

    std::shared_ptr<Piece> selectedPiece =
        board.getPieceAt(selectedPos);

    // לחיצה על כלי מאותו הצבע -> החלפת בחירה
    if (clickedPiece != nullptr &&
        selectedPiece != nullptr &&
        clickedPiece->getSide() ==
            selectedPiece->getSide())
    {
        selection.select(pos);

        selection.setLegalMoves(
            gameEngine.getLegalMoves(pos));
    }
    if (selectedPos == pos)
    {
        gameEngine.requestJump(pos);
        selection.clear();
        return;
    }
    else
    {
        MoveResult result =
            gameEngine.requestMove(
                selectedPos,
                pos);

        selection.clear();
    }
}