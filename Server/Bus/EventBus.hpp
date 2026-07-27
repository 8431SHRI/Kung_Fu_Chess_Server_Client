#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

/*
 * @class EventBus
 * @brief Pub/Sub גנרי - כל טיפוס Event יכול להיות ערוץ נפרד (מזוהה לפי std::type_index).
 * Purpose: לנתק Producers (GameSession/Room) מ-Consumers (Score, MoveLog, Sound, Animations),
 *          כדי שאף אחד מהם לא יצטרך להכיר את השני ישירות (ראה CTD26 slide "BUS").
 * Thread-safety: subscribe/unsubscribe/publish בטוחים לקריאה מ-threads שונים (Scheduler thread
 *                מפרסם, Network/Application threads יכולים להירשם/להסיר לפני/במהלך ריצה).
 * Note: publish() מעתיק את רשימת ה-handlers לפני קריאה אליהם, כדי לא להחזיק את ה-mutex
 *       בזמן שקוד "זר" (handler) רץ - מונע deadlock אם handler מנסה publish/subscribe בחזרה.
 */
class EventBus
{
public:
    using SubscriptionId = std::size_t;

    template <typename Event>
    SubscriptionId subscribe(std::function<void(const Event&)> handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        SubscriptionId id = nextId_++;

        auto& slot = handlers_[std::type_index(typeid(Event))];
        slot.push_back({id, std::make_shared<std::function<void(const Event&)>>(std::move(handler))});

        return id;
    }

    template <typename Event>
    void unsubscribe(SubscriptionId id)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = handlers_.find(std::type_index(typeid(Event)));
        if (it == handlers_.end())
        {
            return;
        }

        auto& slot = it->second;
        slot.erase(
            std::remove_if(slot.begin(), slot.end(),
                [id](const Subscriber& s) { return s.id == id; }),
            slot.end());
    }

    template <typename Event>
    void publish(const Event& event) const
    {
        std::vector<Subscriber> snapshot;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            auto it = handlers_.find(std::type_index(typeid(Event)));
            if (it == handlers_.end())
            {
                return;
            }

            snapshot = it->second; // עותק - כדי לשחרר את ה-mutex לפני קריאה ל-handlers
        }

        for (const auto& subscriber : snapshot)
        {
            auto typedHandler = std::static_pointer_cast<std::function<void(const Event&)>>(subscriber.handler);
            (*typedHandler)(event);
        }
    }

private:
    struct Subscriber
    {
        SubscriptionId id;
        std::shared_ptr<void> handler; // בפועל: shared_ptr<function<void(const Event&)>>, נשמר type-erased
    };

    mutable std::mutex mutex_;
    mutable std::unordered_map<std::type_index, std::vector<Subscriber>> handlers_;
    std::size_t nextId_ = 1;
};
