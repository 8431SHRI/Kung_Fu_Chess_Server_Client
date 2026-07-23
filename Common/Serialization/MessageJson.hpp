#pragma once

#include <optional>

#include <nlohmann/json.hpp>

#include "Messages.hpp"

/*
 * אחריות:
 * המרה בין כל הודעות הפרוטוקול
 * לבין JSON.
 *
 * הקלאס אינו מכיר WebSocket,
 * אינו מכיר MessageType,
 * ואינו עוטף payload.
 */
class MessageJson
{
public:

    // ---------- Client -> Server ----------

    static nlohmann::json toJson(
        const LoginMsg& message);

    static std::optional<LoginMsg> fromLoginJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const RegisterMsg& message);

    static std::optional<RegisterMsg> fromRegisterJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const JoinRoomMsg& message);

    static std::optional<JoinRoomMsg> fromJoinRoomJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const MoveMsg& message);

    static std::optional<MoveMsg> fromMoveJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const JumpMsg& message);

    static std::optional<JumpMsg> fromJumpJson(
        const nlohmann::json& json);



    // ---------- Server -> Client ----------

    static nlohmann::json toJson(
        const LoginOkMsg& message);

    static std::optional<LoginOkMsg> fromLoginOkJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const LoginFailMsg& message);

    static std::optional<LoginFailMsg> fromLoginFailJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const MatchFoundMsg& message);

    static std::optional<MatchFoundMsg> fromMatchFoundJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const RoomStateMsg& message);

    static std::optional<RoomStateMsg> fromRoomStateJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const GameEventMsg& message);

    static std::optional<GameEventMsg> fromGameEventJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const GameOverMsg& message);

    static std::optional<GameOverMsg> fromGameOverJson(
        const nlohmann::json& json);

    static nlohmann::json toJson(
        const DisconnectWarningMsg& message);

    static std::optional<DisconnectWarningMsg> fromDisconnectWarningJson(
        const nlohmann::json& json);
};