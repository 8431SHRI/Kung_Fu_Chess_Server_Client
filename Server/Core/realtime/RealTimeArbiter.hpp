#pragma once
#include "Board.hpp"
#include "Motion.hpp"
#include "Jump.hpp"
#include "PiecePhysicsManager.hpp"
#include "PiecePhysics.hpp"
#include "StateTimer.hpp"
#include <optional>
#include <memory>

class GameEngine;

class RealTimeArbiter
{
private:
    Board &board;
    PiecePhysicsManager &physicsManager;
    std::optional<Motion> activeMotion;
    std::optional<Jump> activeJump;
    std::optional<StateTimer> activeStateTimer;
    void processMotionCompletion(GameEngine &engine);
    void handlePawnPromotion(std::shared_ptr<Piece> piece, const Position &pos);
    void processJumpCompletion();
    void processStateTimerCompletion();

public:
    void handleNextState(
        std::shared_ptr<Piece> piece,
        const PiecePhysics &physics,
        int currentTime);
    RealTimeArbiter(
        Board &b,
        PiecePhysicsManager &physics)
        : board(b),
          physicsManager(physics)
    {
    }

    // חתימה מעודכנת: הארביטר דואג לחישוב הזמן הפנימי
    void handleStateTimerLogic(int currentTime);
    void startMotion(std::shared_ptr<Piece> piece, Position src, Position dst, int startTime);
    void advanceTime(int currentTime, GameEngine &engine);
    bool hasActiveMotion() const;
    void startJump(std::shared_ptr<Piece> piece, int startTime, int jumpDuration);
    void handleMotionLogic(int currentTime, GameEngine &engine);
    void handleJumpLogic(int currentTime);
    void resolveMotion(GameEngine &engine);
    bool isCollisionWithJump(const Position &pos) const;
    void executeStandardMove(GameEngine &engine);
    void handleJumpCollision(std::shared_ptr<Piece> movingPiece);
    const std::optional<Motion> &getActiveMotion() const;
};