#pragma once

#include <functional>
#include <string>

/*
 * @class SpectatorSession
 * @brief חיבור צופה - read-only. לא מחזיק IPlayerSource, ולכן אין דרך "לזוז" דרכו.
 * Purpose: מייצג צופה בתוך Room; מקבל snapshot/event בלבד.
 * Note: החסימה של MOVE היא באמצעות ההיעדר של playerSource (ראו JoinResult ב-Room) -
 *       Application/ConnectionHandler לא אמורים בכלל לנסות לנתב MOVE לצופה, כי
 *       אין להם NetworkPlayerSource לעשות את זה איתו.
 */
class SpectatorSession
{
public:
    using SendCallback = std::function<void(const std::string& rawJson)>;

    SpectatorSession(std::string connectionId, SendCallback sendCallback);

    void send(const std::string& rawJson) const;

    const std::string& getConnectionId() const { return connectionId_; }

private:
    std::string connectionId_;
    SendCallback sendCallback_;
};