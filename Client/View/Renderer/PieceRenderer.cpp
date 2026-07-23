#include "PieceRenderer.hpp"

#include "GameConfig.hpp"

PieceRenderer::PieceRenderer(
    ResourceManager& resourceManager)
    :
    resources(resourceManager)
{
}

void PieceRenderer::drawPiece(
    const PieceSnapshot& piece,
    Img& canvas,
    int elapsedMs)
{
    Animation& animation =
        resources.getAnimation(
            piece.type,
            piece.side,
            piece.state,
            GameConfig::CELL_SIZE_PX);

    std::shared_ptr<Img> image =
        animation.getFrameByTime(
            elapsedMs);

    double x;
    double y;

    if (piece.isMoving)
    {
        double startX =
            piece.motionSource.getCol() *
            GameConfig::CELL_SIZE_PX;

        double startY =
            piece.motionSource.getRow() *
            GameConfig::CELL_SIZE_PX;

        double endX =
            piece.motionDestination.getCol() *
            GameConfig::CELL_SIZE_PX;

        double endY =
            piece.motionDestination.getRow() *
            GameConfig::CELL_SIZE_PX;

        x =
            startX +
            (endX - startX) *
            piece.progress;

        y =
            startY +
            (endY - startY) *
            piece.progress;
    }
    else
    {
        x =
            piece.boardPosition.getCol() *
            GameConfig::CELL_SIZE_PX;

        y =
            piece.boardPosition.getRow() *
            GameConfig::CELL_SIZE_PX;
    }

    image->draw_transparent(
        canvas.get_mat(),
        static_cast<int>(x),
        static_cast<int>(y));
}