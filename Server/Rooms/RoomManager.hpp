#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Room.hpp"
#include "Scheduler.hpp"
#include "PiecePhysicsManager.hpp"

/*
 * @class RoomManager
 * @brief יצירה/חיפוש חדרים לפי שם.
 * Purpose: מיפוי roomName -> Room. לא מכיל לוגיקת משחק בפועל - זו אחריות Room/GameSession.
 */
class RoomManager
{
public:
    RoomManager(
        std::shared_ptr<Scheduler> scheduler,
        std::shared_ptr<PiecePhysicsManager> physicsManager);

    std::shared_ptr<Room> getOrCreate(const std::string& roomName);
    // בניגוד ל-getOrCreate, לא יוצר חדר חדש - מחזיר nullptr אם roomName לא קיים.
    // ל-MoveCommand אסור ליצור חדר בטעות רק כי מישהו שלח MOVE עם gameId שגוי.
    std::shared_ptr<Room> find(const std::string& roomName) const;
    void remove(const std::string& roomName);

private:
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<PiecePhysicsManager> physicsManager_;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms_;
};