#pragma once

#include "GameController.hpp"
/*
 * @class InputManager
 * @brief Responsible for capturing user input and passing it to the game controller.
 * Purpose: Separates input layer from game logic; forwards mouse events to GameController.
 * Created by: main() | Used by: Window (via mouseCallback).
 * Functions: InputManager() (initialization), leftClick() (handles click and forwards to controller).
 */
class InputManager
{
private:

    GameController& controller;

public:

    explicit InputManager(
        GameController& c);

    void leftClick(
        int x,
        int y);
};