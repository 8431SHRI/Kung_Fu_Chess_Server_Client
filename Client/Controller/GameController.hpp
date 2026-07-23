#pragma once

#include "Board.hpp"
#include "Position.hpp"
#include "GameEngine.hpp"
#include "BoardMapper.hpp"
#include "SelectionModel.hpp"

/**
 * @class GameController
 * @brief בקר המשחק (Controller).
 * אחראי על קבלת קלטים מהמשתמש (למשל לחיצות עכבר), תרגומם לקואורדינטות הלוח,
 * ופנייה למנוע המשחק (GameEngine) לביצוע הלוגיקה והמהלכים.
 */
class GameController
{
private:

    Board& board;

    GameEngine& gameEngine;

    SelectionModel& selection;

public:

    GameController(
        Board& b,
        GameEngine& ge,
        SelectionModel& sm);

    void jump(
        int pixelX,
        int pixelY);

    void click(
        int pixelX,
        int pixelY);
};