#include "GameSnapshotJson.hpp"

#include "PieceSnapshot.hpp"

nlohmann::json GameSnapshotJson::toJson(
    const GameSnapshot &snapshot)
{
    nlohmann::json json;

    json["rows"] = snapshot.rows;
    json["cols"] = snapshot.cols;
    json["gameOver"] = snapshot.gameOver;

    json["pieces"] = nlohmann::json::array();

    for (const PieceSnapshot &piece : snapshot.pieces)
    {
        nlohmann::json p;

        p["id"] = piece.id;
        p["side"] = static_cast<int>(piece.side);
        p["type"] = static_cast<int>(piece.type);
        p["state"] = static_cast<int>(piece.state);

        p["row"] = piece.boardPosition.row;
        p["col"] = piece.boardPosition.col;

        p["isMoving"] = piece.isMoving;
        p["progress"] = piece.progress;

        if (piece.isMoving)
        {
            p["sourceRow"] =
                piece.motionSource.row;

            p["sourceCol"] =
                piece.motionSource.col;

            p["destinationRow"] =
                piece.motionDestination.row;

            p["destinationCol"] =
                piece.motionDestination.col;
        }

        json["pieces"].push_back(p);
    }

    return json;
}

GameSnapshot GameSnapshotJson::fromJson(
    const nlohmann::json &json)
{
    GameSnapshot snapshot;

    snapshot.rows =
        json.at("rows").get<int>();

    snapshot.cols =
        json.at("cols").get<int>();

    snapshot.gameOver =
        json.at("gameOver").get<bool>();

    for (const auto &p : json.at("pieces"))
    {
        PieceSnapshot piece;

        piece.id =
            p.at("id").get<int>();

        piece.side =
            static_cast<Side>(
                p.at("side").get<int>());

        piece.type =
            static_cast<PieceType>(
                p.at("type").get<int>());

        piece.state =
            static_cast<PieceState>(
                p.at("state").get<int>());

        piece.boardPosition =
            Position(
                p.at("row").get<int>(),
                p.at("col").get<int>());

        piece.isMoving =
            p.at("isMoving").get<bool>();

        piece.progress =
            p.at("progress").get<double>();

        if (piece.isMoving)
        {
            piece.motionSource =
                Position(
                    p.at("sourceRow").get<int>(),
                    p.at("sourceCol").get<int>());

            piece.motionDestination =
                Position(
                    p.at("destinationRow").get<int>(),
                    p.at("destinationCol").get<int>());
        }

        snapshot.pieces.push_back(piece);
    }

    return snapshot;
}