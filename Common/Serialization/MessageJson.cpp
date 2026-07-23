#include "MessageJson.hpp"

//
// ============================================================
// Client -> Server
// ============================================================
//

nlohmann::json MessageJson::toJson(const LoginMsg& m)
{
    return
    {
        {"username", m.username},
        {"password", m.password}
    };
}

std::optional<LoginMsg>
MessageJson::fromLoginJson(const nlohmann::json& j)
{
    try
    {
        return LoginMsg
        {
            j.at("username"),
            j.at("password")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const RegisterMsg& m)
{
    return
    {
        {"username", m.username},
        {"password", m.password}
    };
}

std::optional<RegisterMsg>
MessageJson::fromRegisterJson(const nlohmann::json& j)
{
    try
    {
        return RegisterMsg
        {
            j.at("username"),
            j.at("password")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const JoinRoomMsg& m)
{
    return
    {
        {"roomName", m.roomName}
    };
}

std::optional<JoinRoomMsg>
MessageJson::fromJoinRoomJson(const nlohmann::json& j)
{
    try
    {
        return JoinRoomMsg
        {
            j.at("roomName")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const MoveMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"fromRow", m.from.row},
        {"fromCol", m.from.col},
        {"toRow", m.to.row},
        {"toCol", m.to.col}
    };
}

std::optional<MoveMsg>
MessageJson::fromMoveJson(const nlohmann::json& j)
{
    try
    {
        return MoveMsg
        {
            j.at("gameId"),

            Position(
                j.at("fromRow"),
                j.at("fromCol")),

            Position(
                j.at("toRow"),
                j.at("toCol"))
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const JumpMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"row", m.position.row},
        {"col", m.position.col}
    };
}

std::optional<JumpMsg>
MessageJson::fromJumpJson(const nlohmann::json& j)
{
    try
    {
        return JumpMsg
        {
            j.at("gameId"),

            Position(
                j.at("row"),
                j.at("col"))
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

//
// ============================================================
// Server -> Client
// ============================================================
//

nlohmann::json MessageJson::toJson(const LoginOkMsg& m)
{
    return
    {
        {"userId", m.userId},
        {"username", m.username},
        {"elo", m.elo}
    };
}

std::optional<LoginOkMsg>
MessageJson::fromLoginOkJson(const nlohmann::json& j)
{
    try
    {
        return LoginOkMsg
        {
            j.at("userId"),
            j.at("username"),
            j.at("elo")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const LoginFailMsg& m)
{
    return
    {
        {"reason", m.reason}
    };
}

std::optional<LoginFailMsg>
MessageJson::fromLoginFailJson(const nlohmann::json& j)
{
    try
    {
        return LoginFailMsg
        {
            j.at("reason")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const MatchFoundMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"opponentName", m.opponentName},
        {"opponentElo", m.opponentElo}
    };
}

std::optional<MatchFoundMsg>
MessageJson::fromMatchFoundJson(const nlohmann::json& j)
{
    try
    {
        return MatchFoundMsg
        {
            j.at("gameId"),
            j.at("opponentName"),
            j.at("opponentElo")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const RoomStateMsg& m)
{
    nlohmann::json players = nlohmann::json::array();

    for (const auto& player : m.players)
    {
        players.push_back(
        {
            {"username", player.username},
            {"elo", player.elo},
            {"isSpectator", player.isSpectator}
        });
    }

    return
    {
        {"roomName", m.roomName},
        {"players", players}
    };
}

std::optional<RoomStateMsg>
MessageJson::fromRoomStateJson(const nlohmann::json& j)
{
    try
    {
        RoomStateMsg room;

        room.roomName = j.at("roomName");

        for (const auto& player : j.at("players"))
        {
            room.players.push_back(
            {
                player.at("username"),
                player.at("elo"),
                player.at("isSpectator")
            });
        }

        return room;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const GameEventMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"eventType", m.eventType},
        {"row", m.at.row},
        {"col", m.at.col}
    };
}

std::optional<GameEventMsg>
MessageJson::fromGameEventJson(const nlohmann::json& j)
{
    try
    {
        return GameEventMsg
        {
            j.at("gameId"),
            j.at("eventType"),

            Position(
                j.at("row"),
                j.at("col"))
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const GameOverMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"winnerUserId", m.winnerUserId},
        {"reason", m.reason}
    };
}

std::optional<GameOverMsg>
MessageJson::fromGameOverJson(const nlohmann::json& j)
{
    try
    {
        return GameOverMsg
        {
            j.at("gameId"),
            j.at("winnerUserId"),
            j.at("reason")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}

nlohmann::json MessageJson::toJson(const DisconnectWarningMsg& m)
{
    return
    {
        {"gameId", m.gameId},
        {"secondsLeft", m.secondsLeft}
    };
}

std::optional<DisconnectWarningMsg>
MessageJson::fromDisconnectWarningJson(const nlohmann::json& j)
{
    try
    {
        return DisconnectWarningMsg
        {
            j.at("gameId"),
            j.at("secondsLeft")
        };
    }
    catch (...)
    {
        return std::nullopt;
    }
}