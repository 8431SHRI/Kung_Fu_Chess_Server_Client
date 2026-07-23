#pragma once
#include "Position.hpp"
#include "GameConfig.hpp"
#include <optional>

class BoardMapper {
public:
    /**
     * @brief ממיר קואורדינטות פיקסלים למיקום בלוח.
     * מחזיר std::optional כדי לטפל במקרים שבהם הלחיצה מחוץ ללוח.
     */
    static Position pixelToPosition(int x, int y) {
       
        int col = x / GameConfig::CELL_SIZE_PX;
        int row = y / GameConfig::CELL_SIZE_PX;

        return Position(row, col);
    }
};