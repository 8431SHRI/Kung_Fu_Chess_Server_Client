#include "EloCalculator.hpp"

#include <cmath>

std::pair<int, int> EloCalculator::newElo(int ratingA, int ratingB, MatchResult result, int kFactor)
{
    double expectedA = 1.0 / (1.0 + std::pow(10.0, (ratingB - ratingA) / 400.0));
    double expectedB = 1.0 / (1.0 + std::pow(10.0, (ratingA - ratingB) / 400.0));

    double scoreA = (result == MatchResult::A_WINS) ? 1.0 : (result == MatchResult::DRAW ? 0.5 : 0.0);
    double scoreB = 1.0 - scoreA;

    int newRatingA = ratingA + static_cast<int>(std::round(kFactor * (scoreA - expectedA)));
    int newRatingB = ratingB + static_cast<int>(std::round(kFactor * (scoreB - expectedB)));

    return {newRatingA, newRatingB};
}