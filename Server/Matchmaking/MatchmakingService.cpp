#include "MatchmakingService.hpp"

#include "MessageJson.hpp"
#include "JsonSerializer.hpp"

MatchmakingService::MatchmakingService(
    std::shared_ptr<RoomManager> roomManager,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    int eloRange,
    int scanIntervalMs,
    int botFallbackMs)
    : roomManager_(std::move(roomManager))
    , sessionRegistry_(std::move(sessionRegistry))
    , eloRange_(eloRange)
    , scanIntervalMs_(scanIntervalMs)
    , botFallbackMs_(botFallbackMs)
{
}

MatchmakingService::~MatchmakingService()
{
    stop();
}

void MatchmakingService::enqueue(
    const std::string& userId,
    const std::string& username,
    int elo,
    const std::string& connectionId,
    std::function<void(const std::string&)> sendCallback)
{
    if (queue_.contains(userId))
    {
        return; // כבר בתור - לא כפול
    }

    QueueEntry entry;
    entry.userId = userId;
    entry.username = username;
    entry.elo = elo;
    entry.connectionId = connectionId;
    entry.sendCallback = std::move(sendCallback);
    entry.enqueuedAt = std::chrono::steady_clock::now();

    queue_.add(std::move(entry));
}

void MatchmakingService::cancel(const std::string& userId)
{
    queue_.remove(userId);
}

std::string MatchmakingService::generateRoomName()
{
    return "match-" + std::to_string(nextRoomId_.fetch_add(1));
}

void MatchmakingService::createMatch(const QueueEntry& a, const QueueEntry& b)
{
    std::string roomName = generateRoomName();
    auto room = roomManager_->getOrCreate(roomName);

    room->join(a.connectionId, a.userId, a.username, a.elo, a.sendCallback);
    room->join(b.connectionId, b.userId, b.username, b.elo, b.sendCallback);

    sessionRegistry_->bindGame(a.connectionId, roomName);
    sessionRegistry_->bindGame(b.connectionId, roomName);

    MatchFoundMsg msgForA{roomName, b.username, b.elo};
    MatchFoundMsg msgForB{roomName, a.username, a.elo};

    a.sendCallback(JsonSerializer::wrap(MessageType::MATCH_FOUND, MessageJson::toJson(msgForA)));
    b.sendCallback(JsonSerializer::wrap(MessageType::MATCH_FOUND, MessageJson::toJson(msgForB)));

    queue_.remove(a.userId);
    queue_.remove(b.userId);
}

void MatchmakingService::createBotMatch(const QueueEntry& a)
{
    std::string roomName = generateRoomName();
    auto room = roomManager_->getOrCreate(roomName);

    room->join(a.connectionId, a.userId, a.username, a.elo, a.sendCallback);
    room->fillWithBot();

    sessionRegistry_->bindGame(a.connectionId, roomName);

    MatchFoundMsg msgForA{roomName, "Bot", a.elo};
    a.sendCallback(JsonSerializer::wrap(MessageType::MATCH_FOUND, MessageJson::toJson(msgForA)));

    queue_.remove(a.userId);
}

void MatchmakingService::scanOnce()
{
    auto snapshot = queue_.snapshot();

    for (const auto& entry : snapshot)
    {
        if (!queue_.contains(entry.userId))
        {
            continue; // כבר זווג בסבב הזה (ע"י entry קודם ב-snapshot)
        }

        auto candidates = queue_.findByEloRange(entry.elo, eloRange_, entry.userId);

        if (!candidates.empty())
        {
            createMatch(entry, candidates.front());
            continue;
        }

        auto waitedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - entry.enqueuedAt).count();

        if (waitedMs >= botFallbackMs_)
        {
            createBotMatch(entry);
        }
    }
}

void MatchmakingService::loop()
{
    while (running_.load())
    {
        scanOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(scanIntervalMs_));
    }
}

void MatchmakingService::run()
{
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) { return; }

    workerThread_ = std::thread(&MatchmakingService::loop, this);
}

void MatchmakingService::stop()
{
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false)) { return; }

    if (workerThread_.joinable()) { workerThread_.join(); }
}