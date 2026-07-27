#include "Room.hpp"

#include <algorithm>
#include <thread>
#include <chrono>

#include "Piece.hpp"
#include "MessageJson.hpp"
#include "JsonSerializer.hpp"
#include "GameSnapshotJson.hpp"
#include "BotPlayerSource.hpp"
#include "GameEvents.hpp"

Room::Room(
    std::string name,
    std::shared_ptr<Scheduler> scheduler,
    std::shared_ptr<PiecePhysicsManager> physicsManager,
    std::shared_ptr<IUserRepository> userRepository)
    : name_(std::move(name))
    , scheduler_(std::move(scheduler))
    , physicsManager_(std::move(physicsManager))
    , userRepository_(std::move(userRepository))
    , bus_(std::make_shared<EventBus>())
{
    moveLogSubscriber_ = std::make_unique<MoveLogSubscriber>(bus_);

    if (userRepository_)
    {
        scoreUpdateSubscriber_ = std::make_unique<ScoreUpdateSubscriber>(bus_, userRepository_);
    }

    bus_->subscribe<GameOverEvent>([this](const GameOverEvent& event)
    {
        std::string winnerUserId;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (event.winnerSide.has_value())
            {
                for (const auto& player : players_)
                {
                    if (player.side == *event.winnerSide)
                    {
                        winnerUserId = player.userId;
                        break;
                    }
                }
            }
        }

        GameOverMsg msg{name_, winnerUserId, event.reason};
        broadcast(JsonSerializer::wrap(MessageType::GAME_OVER, MessageJson::toJson(msg)));
    });
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
    const std::string& userId,
    const std::string& username,
    int elo,
    SendCallback sendCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // reconnect: userId כבר יש לו slot בחדר (מנותק כרגע) - מחברים מחדש, לא יוצרים שחקן שלישי
    for (auto& player : players_)
    {
        if (player.userId == userId)
        {
            player.connectionId = connectionId;
            player.sendCallback = sendCallback;
            player.connected = true;
            player.disconnectToken += 1; // מבטל כל טיימר-חסד תלוי-ועומד מהניתוק הקודם

            broadcastRoomStateLocked();

            return JoinResult{JoinRole::PLAYER, player.side, player.source};
        }
    }

    if (players_.size() < 2)
    {
        Side side = players_.empty() ? Side::WHITE : Side::BLACK;

        PlayerSlot slot;
        slot.connectionId = connectionId;
        slot.userId = userId;
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
        name_, std::move(board), std::move(ruleEngine), std::move(arbiter), std::move(engine), bus_);

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

    publishGameStartedLocked(players_[1].username);

    if (scoreUpdateSubscriber_)
    {
        const PlayerSlot& first = players_[0];
        const PlayerSlot& second = players_[1];

        const PlayerSlot& white = (first.side == Side::WHITE) ? first : second;
        const PlayerSlot& black = (first.side == Side::WHITE) ? second : first;

        scoreUpdateSubscriber_->registerGame(
            name_,
            ScoreUpdateSubscriber::PlayerRatingInfo{white.userId, white.username, white.elo, Side::WHITE},
            ScoreUpdateSubscriber::PlayerRatingInfo{black.userId, black.username, black.elo, Side::BLACK});
    }
}

void Room::publishGameStartedLocked(const std::string& blackUsername) const
{
    if (!bus_)
    {
        return;
    }

    const PlayerSlot& first = players_[0];
    const bool firstIsWhite = (first.side == Side::WHITE);

    GameStartedEvent event;
    event.gameId = name_;
    event.whiteUsername = firstIsWhite ? first.username : blackUsername;
    event.blackUsername = firstIsWhite ? blackUsername : first.username;

    bus_->publish(event);
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

    // ללא registerGame ל-ScoreUpdateSubscriber בכוונה - לבוט אין userId אמיתי,
    // אז אין ELO לעדכן עבור המשחק הזה (ראו הערה ב-ScoreUpdateSubscriber.hpp).
    publishGameStartedLocked("Bot");
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

void Room::onConnectionLost(const std::string& connectionId)
{
    bool isPlayer = false;
    int tokenAtScheduleTime = 0;
    Side disconnectedSide = Side::WHITE;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = std::find_if(players_.begin(), players_.end(),
            [&](const PlayerSlot& p) { return p.connectionId == connectionId; });

        if (it != players_.end())
        {
            isPlayer = true;
            it->connected = false;
            it->sendCallback = nullptr; // אין למי לשלוח יותר על החיבור הזה
            it->disconnectToken += 1;
            tokenAtScheduleTime = it->disconnectToken;
            disconnectedSide = it->side;
        }
    }

    if (!isPlayer)
    {
        // צופה - מוסר בשקט, בלי טיימר (כפי שסוכם)
        std::lock_guard<std::mutex> lock(mutex_);

        spectators_.erase(
            std::remove_if(spectators_.begin(), spectators_.end(),
                [&](const std::unique_ptr<SpectatorSession>& s) { return s->getConnectionId() == connectionId; }),
            spectators_.end());
        return;
    }

    // שידור התראה מיידית ליריב/צופים - לא חוסם, לא ממתין לתשובה
    DisconnectWarningMsg warning{name_, kDisconnectGraceSeconds};
    broadcast(JsonSerializer::wrap(MessageType::DISCONNECT_WARNING, MessageJson::toJson(warning)));

    std::weak_ptr<Room> weakSelf = weak_from_this();

    std::thread([weakSelf, connectionId, tokenAtScheduleTime, disconnectedSide]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(Room::kDisconnectGraceSeconds));

        auto self = weakSelf.lock();
        if (!self) { return; } // Room כבר נהרס בינתיים

        self->applyDisconnectTimeoutIfStillPending(connectionId, tokenAtScheduleTime, disconnectedSide);
    }).detach();
}

void Room::applyDisconnectTimeoutIfStillPending(
    const std::string& connectionId, int expectedToken, Side loserSide)
{
    std::shared_ptr<GameSession> sessionToForfeit;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = std::find_if(players_.begin(), players_.end(),
            [&](const PlayerSlot& p) { return p.connectionId == connectionId; });

        // אם reconnect כבר קרה (connected==true) או שהיה עוד ניתוק/reconnect אחריו
        // (token השתנה) - הטיימר הזה כבר לא רלוונטי.
        if (it == players_.end() || it->connected || it->disconnectToken != expectedToken)
        {
            return;
        }

        if (!session_)
        {
            return;
        }

        sessionToForfeit = session_;
    }

    sessionToForfeit->forceGameOver(loserSide, "disconnect_timeout");
}