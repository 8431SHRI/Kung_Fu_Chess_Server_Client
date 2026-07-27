#include <gtest/gtest.h>

#include "NetworkPlayerSource.hpp"

TEST(NetworkPlayerSourceTest, NoPendingMoveOrJumpInitially)
{
    NetworkPlayerSource source;

    EXPECT_FALSE(source.hasPendingMove());
    EXPECT_FALSE(source.hasPendingJump());
}

TEST(NetworkPlayerSourceTest, EnqueueMoveSetsPendingMove)
{
    NetworkPlayerSource source;
    source.enqueueMove(Position(1, 1), Position(2, 2));

    EXPECT_TRUE(source.hasPendingMove());
}

TEST(NetworkPlayerSourceTest, RequestMoveReturnsEnqueuedPositionsAndConsumesIt)
{
    NetworkPlayerSource source;
    source.enqueueMove(Position(1, 1), Position(2, 2));

    Position from, to;
    source.requestMove(from, to);

    EXPECT_TRUE(from == Position(1, 1));
    EXPECT_TRUE(to == Position(2, 2));
    EXPECT_FALSE(source.hasPendingMove()); // הצריכה מרוקנת את ה-pending move
}

TEST(NetworkPlayerSourceTest, EnqueueJumpSetsPendingJump)
{
    NetworkPlayerSource source;
    source.enqueueJump(Position(3, 3));

    EXPECT_TRUE(source.hasPendingJump());
}

TEST(NetworkPlayerSourceTest, RequestJumpReturnsEnqueuedPositionAndConsumesIt)
{
    NetworkPlayerSource source;
    source.enqueueJump(Position(3, 3));

    Position pos;
    source.requestJump(pos);

    EXPECT_TRUE(pos == Position(3, 3));
    EXPECT_FALSE(source.hasPendingJump()); // הצריכה מרוקנת את ה-pending jump
}

// מתעד במפורש החלטת עיצוב קיימת: אם מגיעות שתי פקודות MOVE לפני שמישהו
// הספיק לצרוך את הראשונה (למשל Scheduler שעדיין לא הגיע ל-tick הבא),
// רק הפקודה האחרונה נשמרת - "last write wins". זו לא תקלה, אלא התנהגות
// מכוונת של pendingMove_ שהוא std::optional יחיד ולא תור.
TEST(NetworkPlayerSourceTest, SecondEnqueueMoveOverwritesFirst_LastWriteWins)
{
    NetworkPlayerSource source;
    source.enqueueMove(Position(1, 1), Position(2, 2));
    source.enqueueMove(Position(5, 5), Position(6, 6));

    Position from, to;
    source.requestMove(from, to);

    EXPECT_TRUE(from == Position(5, 5));
    EXPECT_TRUE(to == Position(6, 6));
}

TEST(NetworkPlayerSourceTest, SecondEnqueueJumpOverwritesFirst_LastWriteWins)
{
    NetworkPlayerSource source;
    source.enqueueJump(Position(1, 1));
    source.enqueueJump(Position(7, 7));

    Position pos;
    source.requestJump(pos);

    EXPECT_TRUE(pos == Position(7, 7));
}

TEST(NetworkPlayerSourceTest, MoveAndJumpArePendingIndependently)
{
    NetworkPlayerSource source;
    source.enqueueMove(Position(1, 1), Position(2, 2));
    source.enqueueJump(Position(3, 3));

    EXPECT_TRUE(source.hasPendingMove());
    EXPECT_TRUE(source.hasPendingJump());

    Position from, to, jumpPos;
    source.requestMove(from, to);

    // צריכת ה-move לא אמורה להשפיע על ה-jump הממתין בנפרד
    EXPECT_TRUE(source.hasPendingJump());
    source.requestJump(jumpPos);
    EXPECT_FALSE(source.hasPendingJump());
}