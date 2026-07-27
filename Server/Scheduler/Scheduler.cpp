#include "Scheduler.hpp"

Scheduler::Scheduler(int tickIntervalMs)
    : tickIntervalMs_(tickIntervalMs)
{
}

Scheduler::~Scheduler()
{
    stop();
}

void Scheduler::run()
{
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true))
    {
        return; // כבר רץ — קריאה כפולה ל-run() היא no-op
    }

    workerThread_ = std::thread(&Scheduler::loop, this);
}

void Scheduler::stop()
{
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false))
    {
        return; // כבר עצור — בטוח לקרוא פעמיים
    }

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

void Scheduler::registerSession(std::shared_ptr<GameSession> session)
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    sessions_[session->getGameId()] = std::move(session);
}

void Scheduler::unregisterSession(const std::string& gameId)
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    sessions_.erase(gameId);
}

std::vector<std::shared_ptr<GameSession>> Scheduler::snapshotActiveSessions() const
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);

    std::vector<std::shared_ptr<GameSession>> result;
    result.reserve(sessions_.size());

    for (const auto& [gameId, session] : sessions_)
    {
        result.push_back(session);
    }

    return result;
}

void Scheduler::loop()
{
    using clock = std::chrono::steady_clock;

    auto nextTick = clock::now();

    while (running_.load())
    {
        auto tickStart = clock::now();

        // מעתיקים את רשימת ה-sessions לפני האיטרציה, כדי לא להחזיק את ה-mutex
        // לאורך כל ה-tick (register/unregister יכולים לקרות מ-thread אחר במקביל)
        auto activeSessions = snapshotActiveSessions();

        for (auto& session : activeSessions)
        {
            if (!session->isGameOver())
            {
                session->tick(tickIntervalMs_);
            }
        }

        nextTick += std::chrono::milliseconds(tickIntervalMs_);

        if (nextTick > tickStart)
        {
            std::this_thread::sleep_until(nextTick);
        }
        else
        {
            // ה-tick לקח יותר זמן מהמוקצב — לא ננסה "להדביק" בקפיצות, פשוט ממשיכים
            nextTick = clock::now();
        }
    }
}