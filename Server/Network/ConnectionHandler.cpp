#include "ConnectionHandler.hpp"

#include "JsonDeserializer.hpp"
#include "JsonSerializer.hpp"

ConnectionHandler::ConnectionHandler(
    std::string connectionId,
    std::shared_ptr<CommandDispatcher> dispatcher,
    std::shared_ptr<SessionRegistry> sessionRegistry,
    SendCallback sendCallback)
    : connectionId_(std::move(connectionId))
    , dispatcher_(std::move(dispatcher))
    , sessionRegistry_(std::move(sessionRegistry))
    , sendCallback_(std::move(sendCallback))
{
}

void ConnectionHandler::handleMessage(const std::string& rawJson)
{
    MessageType type = JsonDeserializer::peekType(rawJson);
    auto payloadOpt = JsonDeserializer::getPayload(rawJson);
    nlohmann::json payload = payloadOpt.has_value() ? *payloadOpt : nlohmann::json::object();

    CommandResult result = dispatcher_->dispatch(connectionId_, type, payload, sendCallback_);

    trackSessionIfLoginSucceeded(result);

    std::string outgoing = JsonSerializer::wrap(result.type, result.payload);
    sendCallback_(outgoing);
}

void ConnectionHandler::trackSessionIfLoginSucceeded(const CommandResult& result)
{
    if (result.type != MessageType::LOGIN_OK)
    {
        return;
    }

    if (result.payload.contains("userId"))
    {
        sessionRegistry_->bindUser(connectionId_, result.payload.at("userId").get<std::string>());
    }
}

void ConnectionHandler::onDisconnect()
{
    dispatcher_->handleDisconnect(connectionId_);
}