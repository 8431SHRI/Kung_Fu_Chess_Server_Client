#pragma once

#include <array>
#include <string>

enum class MessageType
{
    LOGIN,
    REGISTER,
    PLAY_RANDOM,
    CANCEL_MATCH,
    JOIN_ROOM,
    LEAVE_ROOM,
    MOVE,
    JUMP,
    LOGOUT,
    RECONNECT,
    PING,

    LOGIN_OK,
    LOGIN_FAIL,
    MATCH_FOUND,
    ROOM_STATE,
    GAME_SNAPSHOT,
    GAME_EVENT,
    GAME_OVER,
    DISCONNECT_WARNING,
    ERROR,
    PONG,

    UNKNOWN
};

inline constexpr std::array<const char*, 22> MessageTypeNames =
{
    "LOGIN",
    "REGISTER",
    "PLAY_RANDOM",
    "CANCEL_MATCH",
    "JOIN_ROOM",
    "LEAVE_ROOM",
    "MOVE",
    "JUMP",
    "LOGOUT",
    "RECONNECT",
    "PING",

    "LOGIN_OK",
    "LOGIN_FAIL",
    "MATCH_FOUND",
    "ROOM_STATE",
    "GAME_SNAPSHOT",
    "GAME_EVENT",
    "GAME_OVER",
    "DISCONNECT_WARNING",
    "ERROR",
    "PONG",

    "UNKNOWN"
};
inline std::string toString(MessageType type)
{
    return MessageTypeNames[static_cast<size_t>(type)];
}
inline MessageType fromString(const std::string& str)
{
    for (size_t i = 0; i < MessageTypeNames.size(); ++i)
    {
        if (str == MessageTypeNames[i])
        {
            return static_cast<MessageType>(i);
        }
    }

    return MessageType::UNKNOWN;
}