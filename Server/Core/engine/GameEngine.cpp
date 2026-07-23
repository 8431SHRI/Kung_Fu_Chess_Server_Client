#include "GameEngine.hpp"
MoveResult GameEngine::requestMove(const Position &from, const Position &to)
{
    if (isGameOver)
        return {false, "game_over"};
    if (realTimeArbiter.hasActiveMotion())
        return {false, "MOTION_IN_PROGRESS"};

    MoveValidation validation = ruleEngine.isValidMove(to, from, board);
    if (!validation.is_valid)
        return {false, validation.reason};

    // השינוי: הארביטר מחשב את הזמן בעצמו, המנוע רק פוקד עליו להתחיל
    realTimeArbiter.startMotion(board.getPieceAt(from), from, to, currentTimeMs);

    return {true, "ok"};
}

MoveResult GameEngine::requestJump(const Position &position)
{
    if (isGameOver)
        return {false, "game_over"};

    std::shared_ptr<Piece> selectedPiece = board.getPieceAt(position);
    if (selectedPiece == nullptr)
        return {false, "empty_source"};

    MoveValidation validation = ruleEngine.isValidJump(selectedPiece);
    if (!validation.is_valid)
        return {false, validation.reason};

    realTimeArbiter.startJump(selectedPiece, currentTimeMs, GameConfig::DEFAULT_TRAVEL_TIME_MS);
    return {true, "ok"};
}
GameSnapshot GameEngine::snapshot() const
{
    GameSnapshot snap;

    snap.rows = board.getRows();
    snap.cols = board.getCols();

    snap.gameOver = isGameOver;
    const auto &activeMotion =
        realTimeArbiter.getActiveMotion();
    for (int row = 0; row < board.getRows(); row++)
    {
        for (int col = 0; col < board.getCols(); col++)
        {
            auto piece = board.getPieceAt(Position(row, col));

            if (!piece)
                continue;

            PieceSnapshot ps;

            ps.id = piece->getId();
            ps.side = piece->getSide();
            ps.type = piece->getType();
            ps.state = piece->getState();
            ps.boardPosition = piece->getPosition();
            if (activeMotion.has_value() &&
                activeMotion->movingPiece == piece)
            {
                ps.isMoving = true;

                ps.motionSource =
                    activeMotion->source;

                ps.motionDestination =
                    activeMotion->destination;

                ps.progress =
                    activeMotion->getProgress(
                        currentTimeMs);
            }

            snap.pieces.push_back(ps);
        }
    }

    return snap;
}
std::set<Position> GameEngine::getLegalMoves(
    const Position &from) const
{
    return ruleEngine.getLegalMoves(
        from,
        board);
}