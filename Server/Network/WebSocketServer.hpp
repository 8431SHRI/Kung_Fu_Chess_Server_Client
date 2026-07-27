#pragma once

// חייב להיות *לפני* include כלשהו ל-asio/websocketpp ב-Windows:
// windows.h (שנגרר ע"י asio) מגדיר מאקרו בשם ERROR (מ-wingdi.h, ערך 0),
// שמתנגש עם MessageType::ERROR ושובר את ה-enum בקומפילציה. NOGDI מונע
// מ-wingdi.h להיטען בכלל (לא צריך GDI בשרת headless בלאו הכי).
// WIN32_LEAN_AND_MEAN/NOMINMAX הם best-practice נלווה למניעת עוד התנגשויות דומות.
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
#endif

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "ConnectionHandler.hpp"
#include "CommandDispatcher.hpp"
#include "SessionRegistry.hpp"

/*
 * @class WebSocketServer
 * @brief פתיחת socket, קבלת חיבורים, קריאה/כתיבה גולמית (websocketpp + standalone asio).
 * Purpose: accept loop בלבד. מעביר raw string ל-ConnectionHandler ומקבל ממנו raw
 *          string לשליחה חזרה - לא מפענח תוכן הודעות בעצמו.
 * Note: thread נפרד ל-I/O בלבד (io_service::run) - לא נוגע בלוגיקת משחק.
 */
class WebSocketServer
{
public:
    using Server = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHdl = websocketpp::connection_hdl;

    WebSocketServer(
        uint16_t port,
        std::shared_ptr<CommandDispatcher> dispatcher,
        std::shared_ptr<SessionRegistry> sessionRegistry);

    ~WebSocketServer();

    // מתחיל thread פנימי שמריץ את ה-io loop. לא-חוסם.
    void start();

    // עוצר את השרת ומחכה ל-thread הפנימי (join). בטוח לקרוא פעמיים.
    void stop();

    bool isRunning() const { return running_.load(); }

private:
    uint16_t port_;
    std::shared_ptr<CommandDispatcher> dispatcher_;
    std::shared_ptr<SessionRegistry> sessionRegistry_;

    Server server_;
    std::thread ioThread_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> nextConnectionId_{1};

    mutable std::mutex handlersMutex_;

    // owner_less הוא ההשוואה הנכונה ל-connection_hdl לפי websocketpp - לא לפי כתובת
    std::map<ConnectionHdl, std::shared_ptr<ConnectionHandler>, std::owner_less<ConnectionHdl>> handlersByHdl_;

    std::string generateConnectionId();

    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, Server::message_ptr msg);
};