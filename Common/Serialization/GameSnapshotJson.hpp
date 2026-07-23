#pragma once

#include <nlohmann/json.hpp>

#include "GameSnapshot.hpp"

/*
 * אחריות:
 * המרה בין GameSnapshot ל-JSON ולהפך.
 *
 * הקלאס לא מכיר WebSocket,
 * לא מכיר MessageType,
 * ולא יודע לשלוח הודעות.
 */
class GameSnapshotJson
{
public:

    static nlohmann::json toJson(
        const GameSnapshot& snapshot);

    static GameSnapshot fromJson(
        const nlohmann::json& json);
};