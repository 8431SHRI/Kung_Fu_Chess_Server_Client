#include "WebSocketClient.hpp"

#include <iostream>

WebSocketClient::WebSocketClient()
{
    client_.clear_access_channels(websocketpp::log::alevel::all);
    client_.init_asio();

    client_.set_open_handler([this](websocketpp::connection_hdl hdl)
    {
        hdl_ = hdl;
        connected_.store(true);
        if (onConnect_) { onConnect_(); }
    });

    client_.set_close_handler([this](websocketpp::connection_hdl)
    {
        connected_.store(false);
        if (onDisconnect_) { onDisconnect_(); }
    });

    client_.set_fail_handler([this](websocketpp::connection_hdl)
    {
        connected_.store(false);
        if (onDisconnect_) { onDisconnect_(); }
    });

    client_.set_message_handler([this](websocketpp::connection_hdl, Client::message_ptr msg)
    {
        if (onMessage_) { onMessage_(msg->get_payload()); }
    });
}

WebSocketClient::~WebSocketClient()
{
    stop();
}

void WebSocketClient::connect(const std::string& uri)
{
    websocketpp::lib::error_code ec;
    Client::connection_ptr con = client_.get_connection(uri, ec);

    if (ec)
    {
        std::cerr << "WebSocketClient connect error: " << ec.message() << "\n";
        return;
    }

    client_.connect(con);

    running_.store(true);
    ioThread_ = std::thread([this]() { client_.run(); });
}

void WebSocketClient::send(const std::string& rawJson)
{
    if (!connected_.load()) { return; }

    websocketpp::lib::error_code ec;
    client_.send(hdl_, rawJson, websocketpp::frame::opcode::text, ec);
}

void WebSocketClient::stop()
{
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false)) { return; }

    websocketpp::lib::error_code ec;
    client_.close(hdl_, websocketpp::close::status::normal, "client closing", ec);

    if (ioThread_.joinable()) { ioThread_.join(); }
}