#pragma once

#include <functional>
#include <memory>
#include <string>

#include "CommandDispatcher.hpp"
#include "SessionRegistry.hpp"

/*
 * @class ConnectionHandler
 * @brief פר-חיבור: מפענח הודעה נכנסת ומעביר ל-CommandDispatcher, שולח את התוצאה חזרה.
 * Purpose: routing בלבד. אינו מכיר websocketpp/socket ישירות - מקבל SendCallback
 *          כדי להישאר טסטבילי בלי socket אמיתי.
 * Note: כשמתקבלת תוצאה מסוג LOGIN_OK, רושם את ה-userId ב-SessionRegistry
 *       עבור ה-connectionId הזה.
 */
class ConnectionHandler
{
public:
    using SendCallback = std::function<void(const std::string& rawJson)>;

    ConnectionHandler(
        std::string connectionId,
        std::shared_ptr<CommandDispatcher> dispatcher,
        std::shared_ptr<SessionRegistry> sessionRegistry,
        SendCallback sendCallback);

    // נקרא בכל פעם שמגיעה הודעה גולמית (JSON string) מהחיבור הזה.
    void handleMessage(const std::string& rawJson);

    void onDisconnect();

    const std::string& getConnectionId() const { return connectionId_; }

private:
    std::string connectionId_;
    std::shared_ptr<CommandDispatcher> dispatcher_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;
    SendCallback sendCallback_;

    void trackSessionIfLoginSucceeded(const CommandResult& result);
};