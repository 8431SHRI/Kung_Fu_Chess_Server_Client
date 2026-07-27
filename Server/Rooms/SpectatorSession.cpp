#include "SpectatorSession.hpp"

SpectatorSession::SpectatorSession(std::string connectionId, SendCallback sendCallback)
    : connectionId_(std::move(connectionId))
    , sendCallback_(std::move(sendCallback))
{
}

void SpectatorSession::send(const std::string& rawJson) const
{
    if (sendCallback_)
    {
        sendCallback_(rawJson);
    }
}