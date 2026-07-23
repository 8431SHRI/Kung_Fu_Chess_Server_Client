#pragma once

#include <map>
#include <memory>
#include <string>
#include <filesystem>

#include "Animation.hpp"
#include "AnimationKey.hpp"

class ResourceManager
{
private:

    std::string assetsPath;

    std::map<AnimationKey, Animation> animationCache;

    std::string buildFolder(
        PieceType type,
        Side side,
        PieceState state) const;

    Animation loadAnimation(
        PieceType type,
        Side side,
        PieceState state,
        int size);

public:

    explicit ResourceManager(
        const std::string& assetsFolder);

    void loadAnimationMetadata(
        Animation& animation,
        const std::filesystem::path& folder);

    Animation& getAnimation(
        PieceType type,
        Side side,
        PieceState state,
        int size);
};