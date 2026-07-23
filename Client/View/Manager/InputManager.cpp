#include "InputManager.hpp"

InputManager::InputManager(
    GameController& c)
    :
    controller(c)
{
}

void InputManager::leftClick(
    int x,
    int y)
{
    controller.click(
        x,
        y);
}