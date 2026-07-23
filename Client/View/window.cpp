#include "Window.hpp"

#include "AssetsConfig.hpp"
#include "GameConfig.hpp"
#include "OpenCVConfig.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

Window* Window::instance = nullptr;

Window::Window(
    Renderer& r,
    GameController& c,
    SelectionModel& s)
    :
    renderer(r),
    selection(s),
    controller(c)
{
    instance = this;

    canvas.read(
        AssetsConfig::BOARD_IMAGE,
        {
            GameConfig::BOARD_WIDTH_PX,
            GameConfig::BOARD_HEIGHT_PX
        },
        false);

    if (canvas.get_mat().channels() ==
        OpenCVConfig::RGB_CHANNELS)
    {
        cv::cvtColor(
            canvas.get_mat(),
            canvas.get_mat(),
            cv::COLOR_BGR2BGRA);
    }

    cv::namedWindow(
        AssetsConfig::WINDOW_TITLE);

    cv::setMouseCallback(
        AssetsConfig::WINDOW_TITLE,
        Window::mouseCallback,
        nullptr);
}

void Window::mouseCallback(
    int event,
    int x,
    int y,
    int,
    void*)
{
    if (event != cv::EVENT_LBUTTONDOWN)
        return;

    if (instance == nullptr)
        return;

    instance->controller.click(x, y);
}

void Window::render(
    const GameSnapshot& snapshot,
    int elapsedMs)
{
    renderer.render(
        snapshot,
        selection,
        canvas,
        elapsedMs);
}

void Window::show()
{
    cv::imshow(
        AssetsConfig::WINDOW_TITLE,
        canvas.get_mat());
}

Img& Window::getCanvas()
{
    return canvas;
}