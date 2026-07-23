#pragma once

#include <string>

#include "img.hpp"
#include "Renderer.hpp"
#include "GameController.hpp"
#include "SelectionModel.hpp"

/*
 * @class Window
 * @brief Manages the OpenCV game window and mouse events.
 * Purpose: Displays the rendered frame and forwards user clicks to the controller.
 * Created by: main().
 */

class Window
{
private:

    Img canvas;

    Renderer& renderer;

    SelectionModel& selection;

    GameController& controller;

    static Window* instance;

    static void mouseCallback(
        int event,
        int x,
        int y,
        int flags,
        void* userdata);

public:

    Window(
        Renderer& renderer,
        GameController& controller,
        SelectionModel& selection);

    void render(
        const GameSnapshot& snapshot,
        int elapsedMs);

    void show();

    Img& getCanvas();
};