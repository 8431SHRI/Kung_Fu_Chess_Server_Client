#pragma once

#include <string>
#include <vector>

#include "Position.hpp"

/*
 * קובץ זה מכיל אך ורק מבני נתונים (DTO).
 * אין כאן לוגיקה, ולידציה או גישה למסד נתונים.
 */

//====================================================
// Client -> Server
//====================================================

struct LoginMsg
{
    std::string username;
    std::string password;
};

struct RegisterMsg
{
    std::string username;
    std::string password;
};

struct JoinRoomMsg
{
    std::string roomName;
};

struct MoveMsg
{
    std::string gameId;

    Position from;
    Position to;
};

struct JumpMsg
{
    std::string gameId;

    Position position;
};

//====================================================
// Server -> Client
//====================================================

struct LoginOkMsg
{
    std::string userId;

    std::string username;

    int elo;
};

struct LoginFailMsg
{
    std::string reason;
};

struct MatchFoundMsg
{
    std::string gameId;

    std::string opponentName;

    int opponentElo;
};

struct PlayerInfo
{
    std::string username;

    int elo;

    bool isSpectator;
};

struct RoomStateMsg
{
    std::string roomName;

    std::vector<PlayerInfo> players;
};

struct GameEventMsg
{
    std::string gameId;

    std::string eventType;

    Position at;
};

struct GameOverMsg
{
    std::string gameId;

    std::string winnerUserId;

    std::string reason;
};

struct DisconnectWarningMsg
{
    std::string gameId;

    int secondsLeft;
};