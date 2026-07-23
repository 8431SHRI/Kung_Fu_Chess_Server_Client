#pragma once
#include <iostream>

struct Position {
    int row;
    int col;

    public:
     Position() : row(0), col(0) {}
     Position(int r, int c) : row(r), col(c) {}
     
     int getRow() const { return row; }
     int getCol() const { return col; }

     bool operator==(const Position& other) const {
        return (row == other.row && col == other.col);
    }
    bool operator<(const Position& other) const {
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
};

