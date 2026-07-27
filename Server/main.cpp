#include <csignal>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>


#include "InMemoryUserRepository.hpp"
#include "AuthService.hpp"
#include "CommandDispatcher.hpp"
#include "SessionRegistry.hpp"
#include "RoomManager.hpp"
#include "WebSocketServer.hpp"
#include "Scheduler.hpp"
#include "PiecePhysicsManager.hpp"
#include "MatchmakingService.hpp"

namespace
{
    std::atomic<bool> g_shutdownRequested{false};

    void onSigint(int /*signal*/)
    {
        g_shutdownRequested.store(true);
    }
}

int main(int argc, char** argv)
{
    uint16_t port = 9002;

    if (argc > 1)
    {
        port = static_cast<uint16_t>(std::stoi(argv[1]));
    }

    std::cout << "Kung Fu Chess Server starting on port " << port << "...\n";

    // ------------------------------------------------------------------
    // שכבת Persistence - InMemoryUserRepository בכוונה, עדיין לא SQLite
    // ------------------------------------------------------------------
    auto userRepository = std::make_shared<InMemoryUserRepository>();

    // ------------------------------------------------------------------
    // שכבת Auth
    // ------------------------------------------------------------------
    auto authService = std::make_shared<AuthService>(userRepository);

    // ------------------------------------------------------------------
    // Scheduler - חייב לרוץ לפני שנרשמים אליו GameSession-ים מ-Rooms
    // ------------------------------------------------------------------
    auto scheduler = std::make_shared<Scheduler>(16);

    // ------------------------------------------------------------------
    // PiecePhysicsManager - נדרש ע"י כל GameSession חדש שנוצר בתוך Room
    // ------------------------------------------------------------------
    auto physicsManager = std::make_shared<PiecePhysicsManager>("assets");

    // ------------------------------------------------------------------
    // שכבת Rooms
    // ------------------------------------------------------------------
    auto roomManager = std::make_shared<RoomManager>(scheduler, physicsManager);

    // ------------------------------------------------------------------
    // שכבת Matchmaking
    // ------------------------------------------------------------------
    auto matchmakingService = std::make_shared<MatchmakingService>(roomManager, sessionRegistry);

    // ------------------------------------------------------------------
    // שכבת Network
    // ------------------------------------------------------------------
    auto sessionRegistry = std::make_shared<SessionRegistry>();

    // ------------------------------------------------------------------
    // שכבת Application
    // ------------------------------------------------------------------
    auto commandDispatcher = std::make_shared<CommandDispatcher>(
        authService, roomManager, sessionRegistry, matchmakingService);

    WebSocketServer webSocketServer(port, commandDispatcher, sessionRegistry);

    // ------------------------------------------------------------------
    // הרצה
    // ------------------------------------------------------------------
    std::signal(SIGINT, onSigint);

    scheduler->run();
    matchmakingService->run();
    webSocketServer.start();

    std::cout << "Server is running. Supported message types: LOGIN, REGISTER, JOIN_ROOM, MOVE, PLAY_RANDOM, CANCEL_MATCH.\n";
    std::cout << "Press Ctrl+C to stop.\n";

    while (!g_shutdownRequested.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nShutting down...\n";

    webSocketServer.stop();
    matchmakingService->stop();
    scheduler->stop();

    std::cout << "Server stopped cleanly.\n";

    return 0;
}