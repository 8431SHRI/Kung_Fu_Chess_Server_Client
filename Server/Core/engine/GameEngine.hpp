#pragma once
#include "Board.hpp"
#include "RuleEngine.hpp"
#include "GameConfig.hpp"
#include "RealTimeArbiter.hpp"
#include "GameSnapshot.hpp"
#include "PieceSnapshot.hpp"
#include <string>
#include <memory>

struct MoveResult
{
    bool success;
    std::string reason;
};

class GameEngine
{
private:
    Board &board;
    RuleEngine &ruleEngine;
    RealTimeArbiter &realTimeArbiter;
    int currentTimeMs;
    bool isGameOver;

public:
    GameEngine(Board &board, RuleEngine &ruleEngine, RealTimeArbiter &arbiter)
        : board(board), ruleEngine(ruleEngine), realTimeArbiter(arbiter),
          currentTimeMs(GameConfig::INITIAL_TIME_MS), isGameOver(false) {}

    const Board &getBoard() const { return board; }
    int getCurrentTime() const { return currentTimeMs; }

    MoveResult requestMove(const Position &from, const Position &to);
    MoveResult requestJump(const Position &position);

    void wait(int milliseconds)
    {
        currentTimeMs += milliseconds;
        realTimeArbiter.advanceTime(currentTimeMs, *this);
    }

    void signalGameOver()
    {
        isGameOver = true;
    }
    GameSnapshot snapshot() const;
    RuleEngine &getRuleEngine()
    {
        return ruleEngine;
    }

    const RuleEngine &getRuleEngine() const
    {
        return ruleEngine;
    }
    std::set<Position> getLegalMoves(
    const Position& from) const;
};