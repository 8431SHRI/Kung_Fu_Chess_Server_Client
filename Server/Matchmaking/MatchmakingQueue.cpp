#include "MatchmakingQueue.hpp"

#include <algorithm>
#include <cmath>

void MatchmakingQueue::add(QueueEntry entry)
{
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.push_back(std::move(entry));
}

void MatchmakingQueue::remove(const std::string& userId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [&](const QueueEntry& e) { return e.userId == userId; }),
        entries_.end());
}

bool MatchmakingQueue::contains(const std::string& userId) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return std::any_of(entries_.begin(), entries_.end(),
        [&](const QueueEntry& e) { return e.userId == userId; });
}

std::vector<QueueEntry> MatchmakingQueue::findByEloRange(int elo, int range, const std::string& excludeUserId) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<QueueEntry> result;

    for (const auto& e : entries_)
    {
        if (e.userId == excludeUserId) { continue; }

        if (std::abs(e.elo - elo) <= range)
        {
            result.push_back(e);
        }
    }

    return result;
}

std::vector<QueueEntry> MatchmakingQueue::snapshot() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

size_t MatchmakingQueue::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_.size();
}