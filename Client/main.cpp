
#include "Board.hpp"
#include "RuleEngine.hpp"
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "GameController.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "SelectionModel.hpp"

#include <opencv2/highgui.hpp>
#include <iostream>

int main()
{
    try
    {
        std::cout << "Starting Kung Fu Chess..." << std::endl;

        // =========================
        // יצירת הלוח
        // =========================
        Board board(8, 8);

        PieceType backRow[] =
            {
                PieceType::ROOK,
                PieceType::KNIGHT,
                PieceType::BISHOP,
                PieceType::QUEEN,
                PieceType::KING,
                PieceType::BISHOP,
                PieceType::KNIGHT,
                PieceType::ROOK};

        for (int col = 0; col < 8; ++col)
        {
            // שחורים
            board.setPieceAt(
                0, col,
                std::make_shared<Piece>(
                    100 + col,
                    Side::BLACK,
                    backRow[col],
                    Position(0, col)));

            board.setPieceAt(
                1, col,
                std::make_shared<Piece>(
                    200 + col,
                    Side::BLACK,
                    PieceType::PAWN,
                    Position(1, col)));

            // לבנים
            board.setPieceAt(
                6, col,
                std::make_shared<Piece>(
                    300 + col,
                    Side::WHITE,
                    PieceType::PAWN,
                    Position(6, col)));

            board.setPieceAt(
                7, col,
                std::make_shared<Piece>(
                    400 + col,
                    Side::WHITE,
                    backRow[col],
                    Position(7, col)));
        }

        // =========================
        // מנוע המשחק
        // =========================
        PiecePhysicsManager physics("assets");
        RuleEngine ruleEngine;
        RealTimeArbiter arbiter(board, physics);
        GameEngine engine(board, ruleEngine, arbiter);

        // =========================
        // בחירה + קלט
        // =========================
        SelectionModel selection;
        GameController controller(
            board,
            engine,
            selection);

        // =========================
        // ציור
        // =========================
        ResourceManager resources("assets");
        Renderer renderer(resources, "assets/board.png");

        Window window(
            renderer,
            controller,
            selection);

        // =========================
        // לולאת המשחק
        // =========================
        while (true)
        {
            engine.wait(16);
            GameSnapshot snapshot = engine.snapshot();

            window.render(snapshot, engine.getCurrentTime());
            window.show();

            // ESC לסיום
            if (cv::waitKey(16) == 27)
                break;
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}