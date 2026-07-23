#include "RuleEngine.hpp"
#include "RuleKing.hpp"
#include "RuleQueen.hpp"
#include "RuleRook.hpp"
#include "RuleBishop.hpp"
#include "RuleKnight.hpp"
#include "RulePawn.hpp"

// אתחול ויצירת אובייקטי החוקים השונים בתוך מפת הניהול
RuleEngine::RuleEngine() {
    moveRules[PieceType::KING]   = std::make_unique<RuleKing>();
    moveRules[PieceType::QUEEN]  = std::make_unique<RuleQueen>();
    moveRules[PieceType::ROOK]   = std::make_unique<RuleRook>();
    moveRules[PieceType::BISHOP] = std::make_unique<RuleBishop>();
    moveRules[PieceType::KNIGHT] = std::make_unique<RuleKnight>();
    moveRules[PieceType::PAWN]   = std::make_unique<RulePawn>();
}

// מימוש בדיקת חוקיות מהלך הכלים על הלוח
MoveValidation RuleEngine::isValidMove(const Position& to, const Position& from, const Board& grid) {
    // 1. בדיקת גבולות הלוח
    if (!grid.isInsideBoard(to)) {
        return {false, "outside_board"};
    }

    // 2. בדיקת קיום כלי במקור
    auto sourcePiece = grid.getPieceAt(from);
    if (!sourcePiece) {
        return {false, "empty_source"};
    }

    // 3. בדיקת "אש ידידותית" - מניעת תנועה למשבצת שבה נמצא כלי מאותו צבע
    auto destinationPiece = grid.getPieceAt(to);
    if (destinationPiece && sourcePiece->getSide() == destinationPiece->getSide()) {
        return {false, "friendly_destination"};
    }

    // 4. בדיקת חוקיות לפי סוג הכלי (שליפת החוק המתאים מהמפה)
    PieceType type = sourcePiece->getType();
    auto legalMoves = moveRules[type]->getLegalDestinations(grid, *sourcePiece);

    // אם מיקום היעד לא נמצא ברשימת היעדים החוקיים של הכלי
    if (legalMoves.find(to) == legalMoves.end()) {
        return {false, "not_valid_path_for_this_type"};
    }

    return {true, "ok"};
}

// מימוש בדיקת זמינות מצב הכלי לקפיצה
MoveValidation RuleEngine::isValidJump(std::shared_ptr<Piece> piece) const {
    if (piece->getState() == PieceState::MOVING) {
        return { false, "piece_is_moving" };
    }
    if (piece->getState() == PieceState::AIRBORNE) {
        return { false, "piece_is_airborne" };
    }
    if (piece->getState() == PieceState::CAPTURED) {
        return { false, "piece_is_captured" };
    }
    return { true, "ok" };
}
// std::set<Position> RuleEngine::getLegalMoves(
//     const Position& from,
//     const Board& board)
// {
//     auto piece = board.getPieceAt(from);

//     if (!piece)
//     {
//         return {};
//     }

//     auto it = moveRules.find(piece->getType());

//     if (it == moveRules.end())
//     {
//         return {};
//     }

//     return it->second->getLegalDestinations(
//         board,
//         *piece);
// }
std::set<Position> RuleEngine::getLegalMoves(
    const Position& from,
    const Board& board)
{
    std::cout << "getLegalMoves for: "
              << from.getRow()
              << ","
              << from.getCol()
              << std::endl;

    auto piece = board.getPieceAt(from);

    if (!piece)
    {
        std::cout << "No piece!" << std::endl;
        return {};
    }

    auto moves =
        moveRules[piece->getType()]->getLegalDestinations(
            board,
            *piece);

    std::cout << "Found "
              << moves.size()
              << " legal moves"
              << std::endl;

    for (const auto& move : moves)
    {
        std::cout << "("
                  << move.getRow()
                  << ","
                  << move.getCol()
                  << ") ";
    }

    std::cout << std::endl;

    return moves;
}
