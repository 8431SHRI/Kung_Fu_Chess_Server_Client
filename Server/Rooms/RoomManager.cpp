#include "RoomManager.hpp"

RoomManager::RoomManager(
    std::shared_ptr<Scheduler> scheduler,
    std::shared_ptr<PiecePhysicsManager> physicsManager,
    std::shared_ptr<IUserRepository> userRepository)
    : scheduler_(std::move(scheduler))
    , physicsManager_(std::move(physicsManager))
    , userRepository_(std::move(userRepository))
{
}

std::shared_ptr<Room> RoomManager::getOrCreate(const std::string& roomName)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = rooms_.find(roomName);

    if (it != rooms_.end())
    {
        return it->second;
    }

    auto room = std::make_shared<Room>(roomName, scheduler_, physicsManager_, userRepository_);
    rooms_[roomName] = room;

    return room;
}

void RoomManager::remove(const std::string& roomName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rooms_.erase(roomName);
}
std::shared_ptr<Room> RoomManager::find(const std::string& roomName) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = rooms_.find(roomName);
    return (it != rooms_.end()) ? it->second : nullptr;
}