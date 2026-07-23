#pragma once

#include <map>
#include <filesystem>

#include "PiecePhysics.hpp"
#include "AnimationKey.hpp"

class PiecePhysicsManager
{
private:

    std::string assetsFolder;

    std::map<AnimationKey, PiecePhysics> cache;

    std::string buildFolder(
        PieceType type,
        Side side,
        PieceState state) const;

    PiecePhysics loadPhysics(
        PieceType type,
        Side side,
        PieceState state);

public:

    explicit PiecePhysicsManager(
        const std::string& assetsFolder);

    PiecePhysics& getPhysics(
        PieceType type,
        Side side,
        PieceState state);
};