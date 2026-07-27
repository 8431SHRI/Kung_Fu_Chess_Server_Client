#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

#include "Scheduler.hpp"
#include "GameSession.hpp"
#include "PiecePhysicsManager.hpp"
#include "Board.hpp"
#include "RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "Piece.hpp"

// הטסטים כאן כרוכים ב-timing/threading אמיתי (Scheduler מריץ thread פנימי),
// אז הם בכוונה סבלניים ועם מרווחי ביטחון רחבים - לא בודקים מספרי tick מדויקים,
// כי זה תלוי-עומס ולא דטרמיניסטי.
class SchedulerTest : public ::testing::Test
{
protected:
    // כמו ב-GameSessionTests: PiecePhysicsManager חייב לחיות לפחות באותו scope
    // כמו ה-GameSession-ים שנוצרים ממנו, כי RealTimeArbiter מחזיק אליו רפרנס.
    PiecePhysicsManager physicsManager{"assets"};

    // בונה GameSession "כמעט אמיתי" (Board/RuleEngine/Arbiter/Engine אמיתיים),
    // עם snapshotCallback שרק סופר כמה פעמים tick() קרה - מונה עקיף לקצב ה-tick.
    std::shared_ptr<GameSession> makeCountingSession(
        const std::string &gameId,
        std::shared_ptr<std::atomic<int>> tickCounter)
    {
        auto board = std::make_unique<Board>(8, 8);
        board->setPieceAt(6, 3, std::make_shared<Piece>(1, Side::WHITE, PieceType::PAWN, Position(6, 3)));

        auto ruleEngine = std::make_unique<RuleEngine>();
        auto arbiter = std::make_unique<RealTimeArbiter>(*board, physicsManager);
        auto engine = std::make_unique<GameEngine>(*board, *ruleEngine, *arbiter);

        auto session = std::make_shared<GameSession>(
            gameId,
            std::move(board),
            std::move(ruleEngine),
            std::move(arbiter),
            std::move(engine));

        session->setSnapshotCallback([tickCounter](const GameSnapshot &)
                                      { tickCounter->fetch_add(1); });

        return session;
    }
};

TEST_F(SchedulerTest, RegisterThenRunCallsTickRepeatedly)
{
    auto counter = std::make_shared<std::atomic<int>>(0);
    auto session = makeCountingSession("game-1", counter);

    Scheduler scheduler(5); // tick מהיר כדי לקבל כמה tick-ים בזמן קצר
    scheduler.registerSession(session);

    scheduler.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    // ~100ms / 5ms אמור לתת בסביבות 20 tick-ים - בודקים גבול תחתון שמרני בלבד
    EXPECT_GE(counter->load(), 3);
}

TEST_F(SchedulerTest, StopPreventsFurtherTicks)
{
    auto counter = std::make_shared<std::atomic<int>>(0);
    auto session = makeCountingSession("game-1", counter);

    Scheduler scheduler(5);
    scheduler.registerSession(session);

    scheduler.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    scheduler.stop();

    int countAfterStop = counter->load();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // אחרי stop() אין עוד thread רץ, אז המונה לא אמור לזוז יותר
    EXPECT_EQ(counter->load(), countAfterStop);
}

TEST_F(SchedulerTest, UnregisterSessionStopsItsTicks)
{
    auto counterA = std::make_shared<std::atomic<int>>(0);
    auto counterB = std::make_shared<std::atomic<int>>(0);
    auto sessionA = makeCountingSession("game-A", counterA);
    auto sessionB = makeCountingSession("game-B", counterB);

    Scheduler scheduler(5);
    scheduler.registerSession(sessionA);
    scheduler.registerSession(sessionB);

    scheduler.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    scheduler.unregisterSession("game-A");
    int countAAtUnregister = counterA->load();
    int countBAtUnregister = counterB->load();

    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    scheduler.stop();

    // game-A לא אמור לזוז כמעט בכלל אחרי ה-unregister (+1 סובלנות ל-tick
    // שכבר היה "בדרך" ברגע ה-unregister עצמו)
    EXPECT_LE(counterA->load(), countAAtUnregister + 1);

    // game-B, לעומת זאת, אמור להמשיך לקבל tick-ים כרגיל
    EXPECT_GT(counterB->load(), countBAtUnregister);
}

TEST_F(SchedulerTest, RunCalledTwiceIsNoOp)
{
    auto counter = std::make_shared<std::atomic<int>>(0);
    auto session = makeCountingSession("game-1", counter);

    Scheduler scheduler(5);
    scheduler.registerSession(session);

    scheduler.run();
    EXPECT_NO_THROW(scheduler.run()); // קריאה שנייה - לא אמורה ליצור thread נוסף/לקרוס

    EXPECT_TRUE(scheduler.isRunning());

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    scheduler.stop();

    EXPECT_FALSE(scheduler.isRunning());
}

TEST_F(SchedulerTest, StopIsSafeToCallTwice)
{
    auto counter = std::make_shared<std::atomic<int>>(0);
    auto session = makeCountingSession("game-1", counter);

    Scheduler scheduler(5);
    scheduler.registerSession(session);

    scheduler.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    scheduler.stop();
    EXPECT_NO_THROW(scheduler.stop()); // קריאה שנייה ל-stop() - בטוחה, לא-חוסמת, לא קורסת
}

TEST_F(SchedulerTest, DestructorStopsRunningSchedulerWithoutHanging)
{
    auto counter = std::make_shared<std::atomic<int>>(0);
    auto session = makeCountingSession("game-1", counter);

    {
        Scheduler scheduler(5);
        scheduler.registerSession(session);
        scheduler.run();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        // יוצאים מה-scope בלי לקרוא ל-stop() במפורש - ה-destructor אמור
        // לעצור ולעשות join ל-thread הפנימי. אם זה לא קורה, הטסט הזה ייתקע.
    }

    SUCCEED();
}