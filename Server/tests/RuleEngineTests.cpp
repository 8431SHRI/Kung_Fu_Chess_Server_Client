#include <gtest/gtest.h>
#include <memory>

#include "Board.hpp"
#include "RuleEngine.hpp"
#include "Piece.hpp"
#include "Position.hpp"

class RuleEngineTest : public ::testing::Test
{
protected:
    Board board{8, 8};
    RuleEngine ruleEngine;

    void placePiece(int row, int col, Side side, PieceType type)
    {
        board.setPieceAt(row, col, std::make_shared<Piece>(1, side, type, Position(row, col)));
    }
};

// ---------- Knight ----------

TEST_F(RuleEngineTest, KnightInCenterOfEmptyBoardHas8LegalMoves)
{
    placePiece(4, 4, Side::WHITE, PieceType::KNIGHT);

    auto moves = ruleEngine.getLegalMoves(Position(4, 4), board);

    EXPECT_EQ(moves.size(), 8u);
}

TEST_F(RuleEngineTest, KnightInCornerHas2LegalMoves)
{
    placePiece(0, 0, Side::WHITE, PieceType::KNIGHT);

    auto moves = ruleEngine.getLegalMoves(Position(0, 0), board);

    EXPECT_EQ(moves.size(), 2u);
}

TEST_F(RuleEngineTest, KnightCannotLandOnFriendlyPiece)
{
    placePiece(4, 4, Side::WHITE, PieceType::KNIGHT);
    placePiece(6, 5, Side::WHITE, PieceType::PAWN); // אחד מ-8 יעדי הפרש

    auto moves = ruleEngine.getLegalMoves(Position(4, 4), board);

    EXPECT_EQ(moves.count(Position(6, 5)), 0u);
    EXPECT_EQ(moves.size(), 7u);
}

TEST_F(RuleEngineTest, KnightCanCaptureEnemyPiece)
{
    placePiece(4, 4, Side::WHITE, PieceType::KNIGHT);
    placePiece(6, 5, Side::BLACK, PieceType::PAWN);

    auto moves = ruleEngine.getLegalMoves(Position(4, 4), board);

    EXPECT_EQ(moves.count(Position(6, 5)), 1u);
    EXPECT_EQ(moves.size(), 8u);
}

// ---------- Pawn ----------

TEST_F(RuleEngineTest, WhitePawnOnStartRowCanMoveOneOrTwoSquares)
{
    // rows-2 = 6 היא שורת ההתחלה של לבנים על לוח 8x8 (ראו RulePawn::addForwardMoves)
    placePiece(6, 3, Side::WHITE, PieceType::PAWN);

    auto moves = ruleEngine.getLegalMoves(Position(6, 3), board);

    EXPECT_EQ(moves.count(Position(5, 3)), 1u);
    EXPECT_EQ(moves.count(Position(4, 3)), 1u);
    EXPECT_EQ(moves.size(), 2u);
}

TEST_F(RuleEngineTest, WhitePawnNotOnStartRowCanOnlyMoveOneSquare)
{
    placePiece(5, 3, Side::WHITE, PieceType::PAWN);

    auto moves = ruleEngine.getLegalMoves(Position(5, 3), board);

    EXPECT_EQ(moves.count(Position(4, 3)), 1u);
    EXPECT_EQ(moves.size(), 1u);
}

TEST_F(RuleEngineTest, PawnBlockedDirectlyAheadCannotAdvance)
{
    placePiece(6, 3, Side::WHITE, PieceType::PAWN);
    placePiece(5, 3, Side::BLACK, PieceType::PAWN); // חוסם את הצעד הקדימה

    auto moves = ruleEngine.getLegalMoves(Position(6, 3), board);

    EXPECT_EQ(moves.size(), 0u);
}

TEST_F(RuleEngineTest, PawnCanCaptureDiagonally)
{
    placePiece(6, 3, Side::WHITE, PieceType::PAWN);
    placePiece(5, 4, Side::BLACK, PieceType::PAWN);

    auto moves = ruleEngine.getLegalMoves(Position(6, 3), board);

    EXPECT_EQ(moves.count(Position(5, 4)), 1u);
}

// ---------- isValidMove / edge cases ----------

TEST_F(RuleEngineTest, IsValidMoveFailsForEmptySource)
{
    auto result = ruleEngine.isValidMove(Position(4, 4), Position(0, 0), board);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "empty_source");
}

TEST_F(RuleEngineTest, IsValidMoveFailsForDestinationOutsideBoard)
{
    placePiece(0, 0, Side::WHITE, PieceType::ROOK);

    auto result = ruleEngine.isValidMove(Position(-1, 0), Position(0, 0), board);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "outside_board");
}

TEST_F(RuleEngineTest, IsValidMoveFailsForFriendlyDestination)
{
    placePiece(4, 4, Side::WHITE, PieceType::ROOK);
    placePiece(4, 6, Side::WHITE, PieceType::PAWN);

    auto result = ruleEngine.isValidMove(Position(4, 6), Position(4, 4), board);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "friendly_destination");
}

TEST_F(RuleEngineTest, IsValidMoveFailsForIllegalPathForPieceType)
{
    // צריח לא זז באלכסון
    placePiece(4, 4, Side::WHITE, PieceType::ROOK);

    auto result = ruleEngine.isValidMove(Position(6, 6), Position(4, 4), board);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "not_valid_path_for_this_type");
}

TEST_F(RuleEngineTest, IsValidMoveSucceedsForLegalRookMove)
{
    placePiece(4, 4, Side::WHITE, PieceType::ROOK);

    auto result = ruleEngine.isValidMove(Position(4, 7), Position(4, 4), board);

    EXPECT_TRUE(result.is_valid);
}

TEST_F(RuleEngineTest, GetLegalMovesOnEmptySquareReturnsEmptySet)
{
    auto moves = ruleEngine.getLegalMoves(Position(3, 3), board);

    EXPECT_TRUE(moves.empty());
}

// ---------- isValidJump ----------

TEST_F(RuleEngineTest, IsValidJumpFailsWhilePieceIsMoving)
{
    auto piece = std::make_shared<Piece>(1, Side::WHITE, PieceType::KNIGHT, Position(4, 4));
    piece->setState(PieceState::MOVING);

    auto result = ruleEngine.isValidJump(piece);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "piece_is_moving");
}

TEST_F(RuleEngineTest, IsValidJumpFailsWhilePieceIsCaptured)
{
    auto piece = std::make_shared<Piece>(1, Side::WHITE, PieceType::KNIGHT, Position(4, 4));
    piece->setState(PieceState::CAPTURED);

    auto result = ruleEngine.isValidJump(piece);

    EXPECT_FALSE(result.is_valid);
    EXPECT_EQ(result.reason, "piece_is_captured");
}

TEST_F(RuleEngineTest, IsValidJumpSucceedsWhilePieceIsIdle)
{
    auto piece = std::make_shared<Piece>(1, Side::WHITE, PieceType::KNIGHT, Position(4, 4));

    auto result = ruleEngine.isValidJump(piece);

    EXPECT_TRUE(result.is_valid);
}