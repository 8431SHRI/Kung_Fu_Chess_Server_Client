#pragma once
#include <set>
#include "Position.hpp" // נדרש הגדרה מלאה כיוון שהוא פרמטר של std::set

// Forward declarations למניעת תלויות הדדיות ושיפור זמן הקומפילציה
class Board;
class Piece;

/**
 * @brief סורק את הלוח בכיוון (rowStep, colStep) עד למכשול ומכניס את המהלכים החוקיים שנמצאו לקבוצה.
 * * @param board לוח המשחק הנוכחי.
 * @param piece הכלי המבצע את המהלך.
 * @param rowStep השינוי בשורה בכל צעד (כיוון אנכי).
 * @param colStep השינוי בעמודה בכל צעד (כיוון אופקי).
 * @param legalMoves הקבוצה אליה יוכנסו המהלכים החוקיים שנמצאו.
 */
void addMovesInDirection(const Board& board, const Piece& piece, 
                         int rowStep, int colStep, 
                         std::set<Position>& legalMoves);