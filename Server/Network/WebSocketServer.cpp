#include "WebSocketServer.hpp"

WebSocketServer::WebSocketServer(
    uint16_t port,
    std::shared_ptr<CommandDispatcher> dispatcher,
    std::shared_ptr<SessionRegistry> sessionRegistry)
    : port_(port)
    , dispatcher_(std::move(dispatcher))
    , sessionRegistry_(std::move(sessionRegistry))
{
    server_.clear_access_channels(websocketpp::log::alevel::all);
    server_.init_asio();

    server_.set_open_handler([this](ConnectionHdl hdl) { onOpen(hdl); });
    server_.set_close_handler([this](ConnectionHdl hdl) { onClose(hdl); });
    server_.set_message_handler([this](ConnectionHdl hdl, Server::message_ptr msg) { onMessage(hdl, msg); });
}

WebSocketServer::~WebSocketServer()
{
    stop();
}

std::string WebSocketServer::generateConnectionId()
{
    return "conn-" + std::to_string(nextConnectionId_.fetch_add(1));
}

void WebSocketServer::onOpen(ConnectionHdl hdl)
{
    std::string connectionId = generateConnectionId();

    auto sendCallback = [this, hdl](const std::string& outgoing)
    {
        websocketpp::lib::error_code ec;
        server_.send(hdl, outgoing, websocketpp::frame::opcode::text, ec);
        // שגיאת שליחה (למשל חיבור שכבר נסגר) - לא קורסים; onClose ינקה את ה-SessionRegistry.
    };

    auto handler = std::make_shared<ConnectionHandler>(
        connectionId, dispatcher_, sessionRegistry_, sendCallback);

    std::lock_guard<std::mutex> lock(handlersMutex_);
    handlersByHdl_[hdl] = handler;
}

void WebSocketServer::onClose(ConnectionHdl hdl)
{
    std::shared_ptr<ConnectionHandler> handler;

    {
        std::lock_guard<std::mutex> lock(handlersMutex_);
        auto it = handlersByHdl_.find(hdl);
        if (it != handlersByHdl_.end())
        {
            handler = it->second;
            handlersByHdl_.erase(it);
        }
    }

    if (handler)
    {
        handler->onDisconnect();
    }
}

void WebSocketServer::onMessage(ConnectionHdl hdl, Server::message_ptr msg)
{
    std::shared_ptr<ConnectionHandler> handler;

    {
        std::lock_guard<std::mutex> lock(handlersMutex_);
        auto it = handlersByHdl_.find(hdl);
        if (it != handlersByHdl_.end())
        {
            handler = it->second;
        }
    }

    if (handler)
    {
        handler->handleMessage(msg->get_payload());
    }
}

void WebSocketServer::start()
{
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true))
    {
        return; // כבר רץ
    }

    server_.listen(port_);
    server_.start_accept();

    ioThread_ = std::thread([this]() { server_.run(); });
}

void WebSocketServer::stop()
{
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false))
    {
        return; // כבר עצור
    }

    websocketpp::lib::error_code ec;
    server_.stop_listening(ec);
    server_.stop();

    if (ioThread_.joinable())
    {
        ioThread_.join();
    }
}