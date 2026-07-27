#include "Room.hpp"

#include <algorithm>

#include "Piece.hpp"
#include "MessageJson.hpp"
#include "JsonSerializer.hpp"
#include "GameSnapshotJson.hpp"
#include "BotPlayerSource.hpp"

Room::Room(
    std::string name,
    std::shared_ptr<Scheduler> scheduler,
    std::shared_ptr<PiecePhysicsManager> physicsManager)
    : name_(std::move(name))
    , scheduler_(std::move(scheduler))
    , physicsManager_(std::move(physicsManager))
{
}

std::unique_ptr<Board> Room::buildStandardBoard()
{
    // מיקום פתיחה סטנדרטי - זהה בדיוק למה שכתוב ב-Client/main.cpp הקיים,
    // כדי לשמור על עקביות בין הגרסה המקומית לגרסת השרת.
    auto board = std::make_unique<Board>(8, 8);

    PieceType backRow[] =
    {
        PieceType::ROOK, PieceType::KNIGHT, PieceType::BISHOP, PieceType::QUEEN,
        PieceType::KING, PieceType::BISHOP, PieceType::KNIGHT, PieceType::ROOK
    };

    for (int col = 0; col < 8; ++col)
    {
        board->setPieceAt(0, col, std::make_shared<Piece>(100 + col, Side::BLACK, backRow[col], Position(0, col)));
        board->setPieceAt(1, col, std::make_shared<Piece>(200 + col, Side::BLACK, PieceType::PAWN, Position(1, col)));
        board->setPieceAt(6, col, std::make_shared<Piece>(300 + col, Side::WHITE, PieceType::PAWN, Position(6, col)));
        board->setPieceAt(7, col, std::make_shared<Piece>(400 + col, Side::WHITE, backRow[col], Position(7, col)));
    }

    return board;
}

JoinResult Room::join(
    const std::string& connectionId,
    const std::string& username,
    int elo,
    SendCallback sendCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (players_.size() < 2)
    {
        Side side = players_.empty() ? Side::WHITE : Side::BLACK;

        PlayerSlot slot;
        slot.connectionId = connectionId;
        slot.username = username;
        slot.elo = elo;
        slot.side = side;
        slot.source = std::make_shared<NetworkPlayerSource>();
        slot.sendCallback = sendCallback;

        players_.push_back(slot);

        if (players_.size() == 2)
        {
            createGameSessionLocked();
        }

        broadcastRoomStateLocked();

        return JoinResult{JoinRole::PLAYER, side, slot.source};
    }

    spectators_.push_back(std::make_unique<SpectatorSession>(connectionId, sendCallback));
    broadcastRoomStateLocked();

    return JoinResult{JoinRole::SPECTATOR, Side::WHITE, nullptr};
}

void Room::ensureSessionCreatedLocked()
{
    if (session_)
    {
        return; // כבר נוצר - idempotent, כדי ש-fillWithBot ו-createGameSessionLocked יוכלו לחלוק אותו
    }

    auto board = buildStandardBoard();
    auto ruleEngine = std::make_unique<RuleEngine>();
    auto arbiter = std::make_unique<RealTimeArbiter>(*board, *physicsManager_);
    auto engine = std::make_unique<GameEngine>(*board, *ruleEngine, *arbiter);

    session_ = std::make_shared<GameSession>(
        name_, std::move(board), std::move(ruleEngine), std::move(arbiter), std::move(engine));

    session_->setPlayerSource(players_[0].side, players_[0].source);

    session_->setSnapshotCallback([this](const GameSnapshot& snapshot)
    {
        std::string outgoing = JsonSerializer::wrap(MessageType::GAME_SNAPSHOT, GameSnapshotJson::toJson(snapshot));
        broadcast(outgoing);
    });

    scheduler_->registerSession(session_);
}

void Room::createGameSessionLocked()
{
    ensureSessionCreatedLocked();
    session_->setPlayerSource(players_[1].side, players_[1].source);
}

void Room::fillWithBot()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (players_.size() != 1 || session_)
    {
        return; // רק כשיש בדיוק שחקן אנושי אחד ממתין וטרם נוצר session
    }

    ensureSessionCreatedLocked();

    Side botSide = (players_[0].side == Side::WHITE) ? Side::BLACK : Side::WHITE;

    auto botSource = std::make_shared<BotPlayerSource>(
        botSide, session_->getRuleEngine(), session_->getBoard());

    session_->setPlayerSource(botSide, botSource);
}

void Room::broadcastRoomStateLocked() const
{
    RoomStateMsg msg;
    msg.roomName = name_;

    for (const auto& player : players_)
    {
        msg.players.push_back(PlayerInfo{player.username, player.elo, false});
    }

    std::string outgoing = JsonSerializer::wrap(MessageType::ROOM_STATE, MessageJson::toJson(msg));

    for (const auto& player : players_)
    {
        if (player.sendCallback) { player.sendCallback(outgoing); }
    }

    for (const auto& spectator : spectators_)
    {
        spectator->send(outgoing);
    }
}

void Room::broadcast(const std::string& rawJson) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& player : players_)
    {
        if (player.sendCallback) { player.sendCallback(rawJson); }
    }

    for (const auto& spectator : spectators_)
    {
        spectator->send(rawJson);
    }
}

void Room::leave(const std::string& connectionId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    players_.erase(
        std::remove_if(players_.begin(), players_.end(),
            [&](const PlayerSlot& p) { return p.connectionId == connectionId; }),
        players_.end());

    spectators_.erase(
        std::remove_if(spectators_.begin(), spectators_.end(),
            [&](const std::unique_ptr<SpectatorSession>& s) { return s->getConnectionId() == connectionId; }),
        spectators_.end());
}

bool Room::isFull() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return players_.size() >= 2;
}

std::shared_ptr<NetworkPlayerSource> Room::findPlayerSource(const std::string& connectionId) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& player : players_)
    {
        if (player.connectionId == connectionId)
        {
            return player.source;
        }
    }

    return nullptr;
}