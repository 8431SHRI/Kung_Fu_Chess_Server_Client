#pragma once

#include <utility>

enum class MatchResult { A_WINS, B_WINS, DRAW };

/*
 * @class EloCalculator
 * @brief פונקציה טהורה לחישוב ELO חדש - נוסחת ELO סטנדרטית עם K-factor.
 * Purpose: אין לה I/O, אין לה DB access - קלט/פלט בלבד.
 */
class EloCalculator
{
public:
    static std::pair<int, int> newElo(int ratingA, int ratingB, MatchResult result, int kFactor = 32);
};