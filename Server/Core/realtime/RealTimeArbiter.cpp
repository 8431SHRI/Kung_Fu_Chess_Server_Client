#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "GameConfig.hpp" // הוספנו את זה כדי לקבל את הקבועים
#include <cmath>
#include <algorithm>

void RealTimeArbiter::startMotion(
    std::shared_ptr<Piece> piece,
    Position source,
    Position destination,
    int startTime)
{
    if (piece->getState() != PieceState::IDLE)
    {
        return;
    }
    // חישוב מרחק התנועה
    int rowDiff = std::abs(destination.getRow() - source.getRow());
    int colDiff = std::abs(destination.getCol() - source.getCol());

    int distance = std::max(rowDiff, colDiff);

    // קריאת נתוני הפיזיקה מה-JSON
    PiecePhysics &physics =
        physicsManager.getPhysics(
            piece->getType(),
            piece->getSide(),
            PieceState::MOVING);

    double speed = physics.speedCellsPerSecond;

    if (speed <= 0.0)
    {
        throw std::runtime_error("Invalid movement speed");
    }

    // חישוב זמן התנועה
    int travelTime =
        static_cast<int>((distance / speed) * 1000.0);

    // התחלת התנועה
    piece->setState(PieceState::MOVING);

    activeMotion.emplace(
        piece,
        source,
        destination,
        startTime,
        travelTime,
        physics.nextState);
}

// ניהול הזמן והבדיקה האם פעולות בזמן אמת הגיעו לסיומן
void RealTimeArbiter::advanceTime(int currentTime, GameEngine &engine)
{
    handleMotionLogic(currentTime, engine);

    handleJumpLogic(currentTime);

    handleStateTimerLogic(currentTime);
}

// ניהול תהליך התנועה (Motion)
void RealTimeArbiter::handleMotionLogic(int currentTime, GameEngine &engine)
{
    
    if (activeMotion.has_value() && activeMotion->hasArrived(currentTime))
    {
        resolveMotion(engine);
        activeMotion.reset();
    }
}

// ניהול תהליך הקפיצה (Jump)
void RealTimeArbiter::handleJumpLogic(int currentTime)
{
    if (activeJump.has_value() && activeJump->hasFinished(currentTime))
    {
        processJumpCompletion();
    }
}

// החלטה על אופן סיום התנועה: האם מדובר במהלך תקין או בהתנגשות עם כלי קופץ?
void RealTimeArbiter::resolveMotion(GameEngine &engine)
{
    Position destination = activeMotion->destination;

    if (isCollisionWithJump(destination))
    {
        handleJumpCollision(activeMotion->movingPiece);
    }
    else
    {
        executeStandardMove(engine);
    }
}

// בדיקה האם היעד של התנועה תפוס על ידי כלי שנמצא ב"קפיצה" (AIRBORNE)
bool RealTimeArbiter::isCollisionWithJump(const Position &pos) const
{
    return activeJump.has_value() && activeJump->getPosition() == pos;
}

// לוגיקה של ביצוע מהלך שחמט רגיל
void RealTimeArbiter::executeStandardMove(GameEngine &engine)
{
    Position destination = activeMotion->destination;
    auto movingPiece = activeMotion->movingPiece;
    auto targetPiece = board.getPieceAt(destination);

    // 1. זיהוי הכאת מלך (סיום משחק)
    if (targetPiece && targetPiece->getType() == PieceType::KING)
    {
        engine.signalGameOver();
    }

    // 2. הסרת הכלי המותקף מהלוח
    if (targetPiece)
    {
        board.removePiece(destination);
    }

    // 3. עדכון מיקום הכלי בלוח
    board.movePiece(
        activeMotion->source,
        destination);

    handlePawnPromotion(
        movingPiece,
        destination);

    PiecePhysics &physics =
        physicsManager.getPhysics(
            movingPiece->getType(),
            movingPiece->getSide(),
            PieceState::MOVING);

    handleNextState(
        movingPiece,
        physics,
        engine.getCurrentTime());
}

// טיפול מיוחד במקרה של אכילה תוך כדי קפיצה
void RealTimeArbiter::handleJumpCollision(
    std::shared_ptr<Piece> movingPiece)
{
    board.removePiece(activeMotion->source);

    activeJump->getPiece()->setState(
        PieceState::IDLE);

    activeJump.reset();
}

// בדיקת רגלי בשורת הסיום
void RealTimeArbiter::handlePawnPromotion(std::shared_ptr<Piece> piece, const Position &pos)
{
    if (piece && piece->getType() == PieceType::PAWN)
    {
        int lastRow = (piece->getSide() == Side::WHITE) ? 0 : board.getRows() - 1;
        if (pos.getRow() == lastRow)
        {
            piece->setType(PieceType::QUEEN);
        }
    }
}

// התחלת מצב AIRBORNE
void RealTimeArbiter::startJump(std::shared_ptr<Piece> piece, int startTime, int jumpDuration)
{
    piece->setState(PieceState::AIRBORNE);
    activeJump.emplace(piece, piece->getPosition(), startTime, jumpDuration);
}

// סיום מצב AIRBORNE וחזרה ל-IDLE
void RealTimeArbiter::processJumpCompletion()
{
    if (activeJump.has_value())
    {
        activeJump->getPiece()->setState(PieceState::IDLE);
        activeJump.reset();
    }
}

bool RealTimeArbiter::hasActiveMotion() const
{
    return activeMotion.has_value();
}
const std::optional<Motion> &
RealTimeArbiter::getActiveMotion() const
{
    return activeMotion;
}
void RealTimeArbiter::handleStateTimerLogic(
    int currentTime)
{
    if (!activeStateTimer.has_value())
    {
        return;
    }

    if (activeStateTimer->hasFinished(currentTime))
    {
        processStateTimerCompletion();
    }
}
void RealTimeArbiter::processStateTimerCompletion()
{
    if (!activeStateTimer.has_value())
    {
        return;
    }

    activeStateTimer->getPiece()->setState(
        activeStateTimer->getNextState());

    activeStateTimer.reset();
}
void RealTimeArbiter::handleNextState(
    std::shared_ptr<Piece> piece,
    const PiecePhysics &physics,
    int currentTime)
{
    std::cout
        << "[State] "
        << "Piece " << piece->getId()
        << " -> SHORT_REST"
        << std::endl;
    // כרגע רק IDLE ו-REST
    if (physics.nextState == PieceState::IDLE)
    {
        piece->setState(PieceState::IDLE);
        return;
    }

    if (physics.nextState == PieceState::SHORT_REST ||
        physics.nextState == PieceState::LONG_REST)
    {
        piece->setState(physics.nextState);

        // כרגע זמן בדיקה.
        // בשלב הבא יגיע מה-JSON.

        PiecePhysics &restPhysics =
            physicsManager.getPhysics(
                piece->getType(),
                piece->getSide(),
                physics.nextState);
        std::cout
            << "[Rest Config] "
            << "duration="
            << restPhysics.restTimeMs
            << std::endl;
        activeStateTimer.emplace(
            piece,
            physics.nextState,
            restPhysics.nextState,
            currentTime,
            restPhysics.restTimeMs);

        return;
    }

    // כל מצב אחר
    piece->setState(physics.nextState);
}