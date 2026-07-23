#include "ResourceManager.hpp"

#include "AssetsConfig.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

ResourceManager::ResourceManager(const std::string& assetsFolder)
    : assetsPath(assetsFolder)
{
}

std::string ResourceManager::buildFolder(
    PieceType type,
    Side side,
    PieceState state) const
{
    fs::path path = assetsPath;

    path /= AssetsConfig::PIECES_FOLDER;

    path /= Piece::getSideFolderName(side);

    path /= Piece::getTypeFolderName(type);

    path /= Piece::getStateFolderName(state);

    path /= AssetsConfig::SPRITES_FOLDER;

    return path.string();
}

Animation ResourceManager::loadAnimation(
    PieceType type,
    Side side,
    PieceState state,
    int size)
{
    Animation animation;

    fs::path folder = buildFolder(
        type,
        side,
        state);

    if (!fs::exists(folder))
    {
        throw std::runtime_error(
            "Animation folder not found: " +
            folder.string());
    }

    std::vector<fs::path> files;

    for (const auto& entry : fs::directory_iterator(folder))
    {
        if (entry.path().extension() ==
            AssetsConfig::IMAGE_EXTENSION)
        {
            files.push_back(entry.path());
        }
    }

    std::sort(
        files.begin(),
        files.end());

    loadAnimationMetadata(
        animation,
        folder);

    for (const auto& file : files)
    {
        auto image =
            std::make_shared<Img>();

        image->read(
            file.string(),
            {size, size},
            false);

        animation.addFrame(image);
    }

    return animation;
}

Animation& ResourceManager::getAnimation(
    PieceType type,
    Side side,
    PieceState state,
    int size)
{
    AnimationKey key
    {
        type,
        side,
        state,
        size
    };

    auto it =
        animationCache.find(key);

    if (it != animationCache.end())
    {
        return it->second;
    }

    animationCache.emplace(
        key,
        loadAnimation(
            type,
            side,
            state,
            size));

    return animationCache.at(key);
}

void ResourceManager::loadAnimationMetadata(
    Animation& animation,
    const fs::path& folder)
{
    fs::path jsonFile =
        folder.parent_path() /
        AssetsConfig::CONFIG_FILE;

    if (!fs::exists(jsonFile))
    {
        return;
    }

    std::ifstream file(jsonFile);

    json j;

    file >> j;

    if (j.contains("graphics"))
    {
        auto graphics = j["graphics"];

        if (graphics.contains("frames_per_sec"))
        {
            animation.setFramesPerSecond(
                graphics["frames_per_sec"]);
        }

        if (graphics.contains("is_loop"))
        {
            animation.setLoop(
                graphics["is_loop"]);
        }
    }
}