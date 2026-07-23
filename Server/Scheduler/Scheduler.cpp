#include "Scheduler.hpp"

#include <vector>

Scheduler::Scheduler(int tickIntervalMs)
    : tickIntervalMs_(tickIntervalMs)
{
}

Scheduler::~Scheduler()
{
    stop();
}

void Scheduler::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;
    thread_ = std::thread(&Scheduler::run, this);
}

void Scheduler::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;

    if (thread_.joinable())
    {
        thread_.join();
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

void Scheduler::run()
{
    using clock = std::chrono::steady_clock;

    auto nextTick = clock::now();

    while (running_)
    {
        nextTick += std::chrono::milliseconds(tickIntervalMs_);

        // עותק קצר-lock של רשימת ה-sessions, כדי לא לחסום registerSession/unregisterSession
        // בזמן שה-tick עצמו רץ.
        std::vector<std::shared_ptr<GameSession>> snapshot;

        {
            std::lock_guard<std::mutex> lock(sessionsMutex_);
            snapshot.reserve(sessions_.size());

            for (auto& [id, session] : sessions_)
            {
                snapshot.push_back(session);
            }
        }

        std::vector<std::string> finishedIds;

        for (auto& session : snapshot)
        {
            session->tick(tickIntervalMs_);

            if (session->isGameOver())
            {
                finishedIds.push_back(session->getGameId());
            }
        }

        if (!finishedIds.empty())
        {
            std::lock_guard<std::mutex> lock(sessionsMutex_);

            for (auto& id : finishedIds)
            {
                sessions_.erase(id);
            }
        }

        std::this_thread::sleep_until(nextTick);
    }
}