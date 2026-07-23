/**
 * @class ScriptRunner
 * @brief אחראית על קריאת פקודות טקסטואליות והפעלת המחלקות המתאימות בשרשרת הלוגית.
 */
#pragma once
#include <iostream>
#include <string>
#include <sstream> 
#include "Board.hpp"
#include "BoardPrinter.hpp"
#include "GameEngine.hpp"
#include "GameController.hpp"
class ScriptRunner {
private:
    std::istream& input;   // הזרם שממנו נקראות הפקודות
    std::ostream& output;  // הזרם אליו נשלח הפלט (למשל למסך)
    GameController& controller;
    GameEngine& gameEngine;
    Board& board;

public:
    // בנאי: קבלת כל הרכיבים שה-Runner צריך כדי לתפעל את המשחק
    ScriptRunner(std::istream& in, std::ostream& out, 
                 GameController& c, GameEngine& ge, Board& b)
        : input(in), output(out), controller(c), gameEngine(ge), board(b) {}

    // פונקציית הריצה הראשית
    void run() {
        std::string line;
        // קריאת הקלט שורה אחר שורה עד סוף הקובץ
        while (std::getline(input, line)) {
            if (line.empty()) continue;

            // שימוש ב-stringstream כדי לפצל את השורה למילים/מספרים
            std::stringstream ss(line);
            std::string command;
            ss >> command;

            // ניתוב לפונקציה המתאימה לפי שם הפקודה
            if (command == "click") {
                int x, y;
                ss >> x >> y; // קריאת הפרמטרים
                executeClick(x, y);
            } else if (command == "wait") {
                int ms;
                ss >> ms;
                executeWait(ms);
            } else if (command == "print") {
                std::string sub;
                ss >> sub; // קריאת המילה "board"
                executePrint();
            }
            else if (command == "jump") {
              int x, y;
              ss >> x >> y;
              executeJump(x, y);
}
        }
    }

private:
    // פונקציות עזר שמבצעות את הניתוב בפועל
    void executeClick(int x, int y) { controller.click(x, y); }
    void executeWait(int ms) { gameEngine.wait(ms); }
    void executePrint() { BoardPrinter::print(board, output); }
    void executeJump(int x, int y) { controller.jump(x, y); }
};