#include <gtest/gtest.h>
#include <algorithm>

#include "GameSession.hpp"
#include "PiecePhysicsManager.hpp"
#include "Board.hpp"
#include "RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "Piece.hpp"

class GameSessionTest : public ::testing::Test
{
protected:
    // PiecePhysicsManager חייב לחיות לפחות באותו scope כמו ה-GameSession שנוצר
    // ממנו, כי RealTimeArbiter מחזיק רפרנס אליו (לא owner). לכן הוא חבר של
    // ה-fixture ולא נבנה בתוך makeSessionWithSinglePawn() כמשתנה מקומי.
    //
    // "assets" הוא נתיב יחסי לתיקיית ה-build, ותלוי בזה שה-CMakeLists מעתיק
    // את Server/assets ל-CMAKE_BINARY_DIR - בדיוק כמו שכבר עובד היום עבור
    // PasswordHasherTests/RuleEngineTests וכו'.
    PiecePhysicsManager physicsManager{"assets"};

    std::unique_ptr<GameSession> makeSessionWithSinglePawn(int pieceId, Position startPos)
    {
        auto board = std::make_unique<Board>(8, 8);
        board->setPieceAt(
            startPos.getRow(), startPos.getCol(),
            std::make_shared<Piece>(pieceId, Side::WHITE, PieceType::PAWN, startPos));

        auto ruleEngine = std::make_unique<RuleEngine>();
        auto arbiter = std::make_unique<RealTimeArbiter>(*board, physicsManager);
        auto engine = std::make_unique<GameEngine>(*board, *ruleEngine, *arbiter);

        return std::make_unique<GameSession>(
            "test-game",
            std::move(board),
            std::move(ruleEngine),
            std::move(arbiter),
            std::move(engine));
    }

    static GameCommand makeMoveCommand(Position from, Position to)
    {
        GameCommand cmd;
        cmd.type = GameCommandType::MOVE;
        cmd.side = Side::WHITE;
        cmd.from = from;
        cmd.to = to;
        return cmd;
    }
};

TEST_F(GameSessionTest, GetGameIdReturnsIdPassedToConstructor)
{
    auto session = makeSessionWithSinglePawn(1, Position(6, 3));

    EXPECT_EQ(session->getGameId(), "test-game");
}

TEST_F(GameSessionTest, IsGameOverFalseInitially)
{
    auto session = makeSessionWithSinglePawn(1, Position(6, 3));

    EXPECT_FALSE(session->isGameOver());
}

TEST_F(GameSessionTest, EnqueuedLegalMoveIsAppliedAfterTick)
{
    auto session = makeSessionWithSinglePawn(42, Position(6, 3));

    GameSnapshot lastSnapshot;
    session->setSnapshotCallback([&](const GameSnapshot &snap)
                                  { lastSnapshot = snap; });

    session->enqueue(makeMoveCommand(Position(6, 3), Position(5, 3)));

    // dtMs גדול בכוונה (5 שניות) כדי לוודא שהתנועה תסתיים במלואה בתוך ה-tick
    // הזה, בלי תלות במהירות המדויקת שמוגדרת ל-pawn/move ב-config.json.
    session->tick(5000);

    auto it = std::find_if(lastSnapshot.pieces.begin(), lastSnapshot.pieces.end(),
                            [](const PieceSnapshot &p)
                            { return p.id == 42; });

    ASSERT_NE(it, lastSnapshot.pieces.end());
    EXPECT_TRUE(it->boardPosition == Position(5, 3));
    EXPECT_FALSE(it->isMoving);
}

TEST_F(GameSessionTest, EnqueuedIllegalMoveDoesNotMovePiece)
{
    auto session = makeSessionWithSinglePawn(7, Position(6, 3));

    GameSnapshot lastSnapshot;
    session->setSnapshotCallback([&](const GameSnapshot &snap)
                                  { lastSnapshot = snap; });

    // חייל לא זז 3 משבצות בבת אחת - RuleEngine אמור לפסול את המהלך,
    // כלומר GameEngine::requestMove לא אמור אפילו להתחיל תנועה.
    session->enqueue(makeMoveCommand(Position(6, 3), Position(3, 3)));

    session->tick(5000);

    auto it = std::find_if(lastSnapshot.pieces.begin(), lastSnapshot.pieces.end(),
                            [](const PieceSnapshot &p)
                            { return p.id == 7; });

    ASSERT_NE(it, lastSnapshot.pieces.end());
    EXPECT_TRUE(it->boardPosition == Position(6, 3));
    EXPECT_FALSE(it->isMoving);
}

TEST_F(GameSessionTest, MultipleTicksWithoutEnqueueDoNotCrash)
{
    auto session = makeSessionWithSinglePawn(1, Position(6, 3));

    for (int i = 0; i < 10; ++i)
    {
        EXPECT_NO_THROW(session->tick(16));
    }
}

TEST_F(GameSessionTest, SnapshotCallbackFiresExactlyOncePerTick)
{
    auto session = makeSessionWithSinglePawn(1, Position(6, 3));

    int callCount = 0;
    session->setSnapshotCallback([&](const GameSnapshot &)
                                  { ++callCount; });

    session->tick(16);
    session->tick(16);
    session->tick(16);

    EXPECT_EQ(callCount, 3);
}

// טסט שמתעד במכוון פער ידוע: כרגע GameSession::tick לא מחובר בפועל ל-
// signalGameOver/isGameOver האמיתיים של GameEngine (יש הערת TODO בקוד -
// GameEngine עדיין חושף רק signalGameOver() כ-setter, בלי getter). לכן
// isGameOver() תמיד false, גם בתרחיש שבו בפועל נאכל מלך. זו לא באג בטסט -
// זו תזכורת מכוונת: ברגע שיתווסף getter ל-GameEngine ו-tick יתחבר אליו,
// יש לעדכן את הטסט הזה כדי לבדוק תרחיש אכילת מלך אמיתי.
TEST_F(GameSessionTest, GameOverNotYetWiredToGameEngine_KnownGap)
{
    auto session = makeSessionWithSinglePawn(1, Position(6, 3));

    session->tick(16);

    EXPECT_FALSE(session->isGameOver());
}