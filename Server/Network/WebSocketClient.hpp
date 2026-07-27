#pragma once

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
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

/*
 * @class WebSocketClient
 * @brief חיבור לשרת, שליחה/קבלה גולמית - צד לקוח, סימטרי ל-WebSocketServer בצד שרת.
 * Purpose: I/O בלבד. לא מפענח תוכן הודעה עסקי - זה תפקיד GameGateway.
 * Note: thread נפרד ל-io loop, בדיוק כמו WebSocketServer.
 */
class WebSocketClient
{
public:
    using MessageCallback = std::function<void(const std::string& rawJson)>;
    using ConnectCallback = std::function<void()>;
    using DisconnectCallback = std::function<void()>;

    WebSocketClient();
    ~WebSocketClient();

    void setOnMessage(MessageCallback callback) { onMessage_ = std::move(callback); }
    void setOnConnect(ConnectCallback callback) { onConnect_ = std::move(callback); }
    void setOnDisconnect(DisconnectCallback callback) { onDisconnect_ = std::move(callback); }

    // מתחבר ל-uri (למשל "ws://localhost:9002"). לא-חוסם - מתחיל thread פנימי.
    void connect(const std::string& uri);

    void send(const std::string& rawJson);

    void stop();

    bool isConnected() const { return connected_.load(); }

private:
    using Client = websocketpp::client<websocketpp::config::asio_client>;

    Client client_;
    websocketpp::connection_hdl hdl_;
    std::thread ioThread_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> running_{false};

    MessageCallback onMessage_;
    ConnectCallback onConnect_;
    DisconnectCallback onDisconnect_;
};