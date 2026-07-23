#include "PiecePhysicsManager.hpp"
#include "GameConfig.hpp"
#include "AssetsConfig.hpp"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

PiecePhysicsManager::PiecePhysicsManager(
    const std::string &folder)
    : assetsFolder(folder)
{
}

std::string PiecePhysicsManager::buildFolder(
    PieceType type,
    Side side,
    PieceState state) const
{
    fs::path path = assetsFolder;

    path /= AssetsConfig::PIECES_FOLDER;

    path /= Piece::getSideFolderName(side);

    path /= Piece::getTypeFolderName(type);

    path /= Piece::getStateFolderName(state);

    return path.string();
}

PiecePhysics PiecePhysicsManager::loadPhysics(
    PieceType type,
    Side side,
    PieceState state)
{
    PiecePhysics data;

    fs::path jsonFile =
        fs::path(buildFolder(type, side, state)) / AssetsConfig::CONFIG_FILE;

    std::ifstream file(jsonFile);
    std::cout
        << "[Loading Physics] "
        << jsonFile
        << std::endl;
    json j;

    file >> j;

    auto physics =
        j["physics"];

    if (physics.contains("speed_m_per_sec"))
    {
        data.speedCellsPerSecond =
            physics["speed_m_per_sec"];
    }
    if (physics.contains("rest_time_ms"))
    {
        data.restTimeMs =
            physics["rest_time_ms"];
    }
    if (physics.contains("next_state_when_finished"))
    {
        data.nextState = stringToState(
            physics["next_state_when_finished"]);
    }
    if (data.restTimeMs == 0)
    {
        switch (state)
        {
        case PieceState::SHORT_REST:
            data.restTimeMs = GameConfig::SHORT_REST_TIME_MS;
            break;

        case PieceState::LONG_REST:
            data.restTimeMs = GameConfig::LONG_REST_TIME_MS;
            break;

        default:
            break;
        }
    }

    std::string next =
        physics["next_state_when_finished"];

    if (next == "idle")
        data.nextState = PieceState::IDLE;

    else if (next == "move")
        data.nextState = PieceState::MOVING;

    else if (next == "jump")
        data.nextState = PieceState::AIRBORNE;

    else if (next == "short_rest")
        data.nextState = PieceState::SHORT_REST;

    else if (next == "long_rest")
        data.nextState = PieceState::LONG_REST;

    else
        throw std::runtime_error(
            "Unknown state");

    return data;
}

PiecePhysics &
PiecePhysicsManager::getPhysics(
    PieceType type,
    Side side,
    PieceState state)
{
    AnimationKey key{
        type,
        side,
        state,
        0};

    auto it =
        cache.find(key);

    if (it != cache.end())
        return it->second;

    cache.emplace(
        key,
        loadPhysics(type, side, state));

    return cache.at(key);
}