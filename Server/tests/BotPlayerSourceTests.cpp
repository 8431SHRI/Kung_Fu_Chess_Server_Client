#include <gtest/gtest.h>

#include "Board.hpp"
#include "RuleEngine.hpp"
#include "BotPlayerSource.hpp"
#include "Piece.hpp"

class BotPlayerSourceTest : public ::testing::Test
{
protected:
    Board board{8, 8};
    RuleEngine ruleEngine;

    void placePiece(int row, int col, Side side, PieceType type)
    {
        board.setPieceAt(row, col, std::make_shared<Piece>(1, side, type, Position(row, col)));
    }
};

TEST_F(BotPlayerSourceTest, HasPendingMoveFalseOnEmptyBoard)
{
    BotPlayerSource bot(Side::WHITE, ruleEngine, board);

    EXPECT_FALSE(bot.hasPendingMove());
}

TEST_F(BotPlayerSourceTest, HasPendingMoveTrueWhenLegalMoveExists)
{
    placePiece(6, 3, Side::WHITE, PieceType::PAWN);

    BotPlayerSource bot(Side::WHITE, ruleEngine, board);

    EXPECT_TRUE(bot.hasPendingMove());
}

TEST_F(BotPlayerSourceTest, HasPendingMoveFalseWhenOnlyEnemyPiecesExist)
{
    placePiece(1, 3, Side::BLACK, PieceType::PAWN);

    // בודקים בוט שמייצג לבן, כשעל הלוח יש רק כלים שחורים
    BotPlayerSource bot(Side::WHITE, ruleEngine, board);

    EXPECT_FALSE(bot.hasPendingMove());
}

TEST_F(BotPlayerSourceTest, RequestMoveAlwaysReturnsMoveThatIsLegalPerRuleEngine)
{
    // כמה כלים לבנים, כדי לתת לבוט כמה אפשרויות בחירה אקראית
    placePiece(6, 0, Side::WHITE, PieceType::PAWN);
    placePiece(6, 1, Side::WHITE, PieceType::PAWN);
    placePiece(7, 1, Side::WHITE, PieceType::KNIGHT);
    placePiece(1, 4, Side::BLACK, PieceType::PAWN); // כלי יריב, לא אמור להיבחר בכלל

    BotPlayerSource bot(Side::WHITE, ruleEngine, board);

    // מריצים כמה פעמים כי הבחירה אקראית - רוצים לתפוס גם מקרי קצה נדירים
    for (int i = 0; i < 50; ++i)
    {
        ASSERT_TRUE(bot.hasPendingMove());

        Position from, to;
        bot.requestMove(from, to);

        auto piece = board.getPieceAt(from);
        ASSERT_NE(piece, nullptr);
        EXPECT_EQ(piece->getSide(), Side::WHITE);

        auto legalMoves = ruleEngine.getLegalMoves(from, board);
        EXPECT_EQ(legalMoves.count(to), 1u);
    }
}

TEST_F(BotPlayerSourceTest, HasPendingJumpAlwaysFalse)
{
    placePiece(6, 3, Side::WHITE, PieceType::PAWN);

    BotPlayerSource bot(Side::WHITE, ruleEngine, board);

    EXPECT_FALSE(bot.hasPendingJump());
}

TEST_F(BotPlayerSourceTest, RequestJumpIsNoOpAndDoesNotCrash)
{
    BotPlayerSource bot(Side::WHITE, ruleEngine, board);
    Position pos(2, 2);

    // no-op בכוונה (ראו הערה ב-BotPlayerSource.cpp) — רק מוודאים שהוא לא קורס
    EXPECT_NO_THROW(bot.requestJump(pos));
}