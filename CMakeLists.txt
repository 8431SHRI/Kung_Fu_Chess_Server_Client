
# # cmake_minimum_required(VERSION 3.20)
# # project(KungFuChessOnline CXX)

# # set(CMAKE_CXX_STANDARD 17)
# # set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # find_package(OpenSSL REQUIRED)
# # find_package(GTest CONFIG REQUIRED)
# # find_package(nlohmann_json CONFIG REQUIRED)
# # find_package(OpenCV REQUIRED)

# # # הגדרת נתיבי Include כלליים
# # set(ALL_INCLUDE_DIRS
# #     Server 
# #     Server/Application 
# #     Server/Auth 
# #     Server/Bot 
# #     Server/Bus
# #     Server/Core 
# #     Server/Core/config 
# #     Server/Core/engine 
# #     Server/Core/model
# #     Server/Core/Physics 
# #     Server/Core/realtime 
# #     Server/Core/rules
# #     Server/GameSession 
# #     Server/Matchmaking 
# #     Server/Network
# #     Server/Persistence 
# #     Server/Rooms 
# #     Server/Scheduler
# #     Common 
# #     Common/DTO 
# #     Common/Serialization 
# #     Common/Messages
# # )

# # # ---- ליבת השרת (ללא קבצי Scheduler ו-Application שפוצלו) ----
# # file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
# #     Server/*.cpp
# #     Common/*.cpp
# # )
# # list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*tests?.*")
# # list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*Scheduler.*")
# # list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*Application.*")

# # add_library(kungfu_core STATIC ${SERVER_SOURCES})
# # target_include_directories(kungfu_core PUBLIC ${ALL_INCLUDE_DIRS})
# # target_link_libraries(kungfu_core PUBLIC
# #     OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json
# # )

# # # ---- ספריית ה-Scheduler הנפרדת ----
# # file(GLOB_RECURSE SERVER_SCHEDULER_SOURCES
# #     ${CMAKE_SOURCE_DIR}/Server/Scheduler/*.cpp
# # )
# # add_library(ServerScheduler STATIC ${SERVER_SCHEDULER_SOURCES})
# # target_include_directories(ServerScheduler PUBLIC ${ALL_INCLUDE_DIRS})
# # target_link_libraries(ServerScheduler PUBLIC kungfu_core)

# # # ---- ספריית ה-Application החדשה ----
# # file(GLOB_RECURSE SERVER_APPLICATION_SOURCES
# #     ${CMAKE_SOURCE_DIR}/Server/Application/*.cpp
# # )
# # add_library(ServerApplication STATIC ${SERVER_APPLICATION_SOURCES})
# # target_include_directories(ServerApplication PUBLIC ${ALL_INCLUDE_DIRS})
# # target_link_libraries(ServerApplication PUBLIC kungfu_core nlohmann_json::nlohmann_json)
# # find_package(websocketpp CONFIG REQUIRED)
# # find_package(asio CONFIG REQUIRED)

# # file(GLOB_RECURSE SERVER_NETWORK_SOURCES
# #     ${CMAKE_SOURCE_DIR}/Server/Network/*.cpp
# # )
# # add_library(ServerNetwork STATIC ${SERVER_NETWORK_SOURCES})
# # target_include_directories(ServerNetwork PUBLIC ${ALL_INCLUDE_DIRS})
# # target_compile_definitions(ServerNetwork PUBLIC ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
# # target_link_libraries(ServerNetwork PUBLIC ServerApplication websocketpp::websocketpp asio::asio)
# # # ---- קוד הקליינט + main() האמיתי של המשחק — executable נפרד ----
# # file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS Client/*.cpp)

# # add_executable(kungfu_chess ${CLIENT_SOURCES})
# # target_include_directories(kungfu_chess PRIVATE
# #     Client Client/Controller Client/Resources Client/View
# #     Client/View/Animation Client/View/Renderer
# #     Client/View/Selection Client/View/Manager
# # )
# # target_link_libraries(kungfu_chess PRIVATE kungfu_core ServerScheduler ServerApplication ${OpenCV_LIBS})

# # # ---- טסטים — מתחברים לליבה, ל-Scheduler ול-Application ----
# # enable_testing()

# # file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS Server/tests/*.cpp)

# # add_executable(server_tests ${TEST_SOURCES})
# # target_link_libraries(server_tests PRIVATE kungfu_core ServerScheduler ServerApplication GTest::gtest GTest::gtest_main)

# # include(GoogleTest)
# # file(COPY ${CMAKE_SOURCE_DIR}/Server/assets DESTINATION ${CMAKE_BINARY_DIR})
# # gtest_discover_tests(server_tests)


# cmake_minimum_required(VERSION 3.20)
# project(KungFuChessOnline CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(OpenSSL REQUIRED)
# find_package(GTest CONFIG REQUIRED)
# find_package(nlohmann_json CONFIG REQUIRED)
# find_package(websocketpp CONFIG REQUIRED)
# find_package(asio CONFIG REQUIRED)
# find_package(OpenCV REQUIRED)
# # הגדרת נתיבי Include כלליים
# set(ALL_INCLUDE_DIRS
#     Server 
#     Server/Application 
#     Server/Auth 
#     Server/Bot 
#     Server/Bus
#     Server/Core 
#     Server/Core/config 
#     Server/Core/engine 
#     Server/Core/model
#     Server/Core/Physics 
#     Server/Core/realtime 
#     Server/Core/rules
#     Server/GameSession 
#     Server/Matchmaking 
#     Server/Network
#     Server/Persistence 
#     Server/Rooms 
#     Server/Scheduler
#     Common 
#     Common/DTO 
#     Common/Serialization 
#     Common/Messages
# )

# # ---- ליבת השרת (ללא קבצי Scheduler ו-Application שפוצלו) ----

# file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
#     Server/*.cpp
#     Common/*.cpp
# )
# list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
# list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*tests?.*")
# list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*Scheduler.*")
# list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*Application.*")

# add_library(kungfu_core STATIC ${SERVER_SOURCES})
# target_include_directories(kungfu_core PUBLIC ${ALL_INCLUDE_DIRS})
# target_link_libraries(kungfu_core PUBLIC
#     OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json
# )
# target_link_libraries(server_tests PRIVATE
#     kungfu_core
#     ServerScheduler
#     ServerApplication
#     ServerNetwork
#     GTest::gtest
#     GTest::gtest_main
# )
# # ---- ספריית ה-Scheduler הנפרדת ----
# file(GLOB_RECURSE SERVER_SCHEDULER_SOURCES
#     ${CMAKE_SOURCE_DIR}/Server/Scheduler/*.cpp
# )
# add_library(ServerScheduler STATIC ${SERVER_SCHEDULER_SOURCES})
# target_include_directories(ServerScheduler PUBLIC ${ALL_INCLUDE_DIRS})
# target_link_libraries(ServerScheduler PUBLIC kungfu_core)

# # ---- ספריית ה-Application החדשה ----
# file(GLOB_RECURSE SERVER_APPLICATION_SOURCES
#     ${CMAKE_SOURCE_DIR}/Server/Application/*.cpp
# )
# add_library(ServerApplication STATIC ${SERVER_APPLICATION_SOURCES})
# target_include_directories(ServerApplication PUBLIC ${ALL_INCLUDE_DIRS})
# target_link_libraries(ServerApplication PUBLIC kungfu_core nlohmann_json::nlohmann_json)
# add_definitions(-DBOOST_ASIO_ENABLE_OLD_SERVICES)
# add_definitions(-DASIO_DISABLE_BOOST_BIND_IS_PLACEHOLDER)
# add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
# # ---- ספריית הרשת של השרת (ServerNetwork) ----
# file(GLOB_RECURSE SERVER_NETWORK_SOURCES
#     ${CMAKE_SOURCE_DIR}/Server/Network/*.cpp
# )
# add_library(ServerNetwork STATIC ${SERVER_NETWORK_SOURCES})
# target_include_directories(ServerNetwork PUBLIC ${ALL_INCLUDE_DIRS})
# target_compile_definitions(ServerNetwork PUBLIC ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
# target_link_libraries(ServerNetwork PUBLIC ServerApplication websocketpp::websocketpp asio::asio)

# # ---- קוד הקליינט + main() האמיתי של המשחק — executable נפרד ----
# file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS Client/*.cpp)

# add_executable(kungfu_chess ${CLIENT_SOURCES})
# target_include_directories(kungfu_chess PRIVATE
#     Client Client/Controller Client/Resources Client/View
#     Client/View/Animation Client/View/Renderer
#     Client/View/Selection Client/View/Manager
# )
# target_link_libraries(kungfu_chess PRIVATE kungfu_core ServerScheduler ServerApplication)
# target_include_directories(kungfu_chess PRIVATE
#     ${OpenCV_INCLUDE_DIRS}
# )
# # ----------------------------------------------------------------------------
# # KungFuChessServer - הרצה אמיתית של השרת
# # ----------------------------------------------------------------------------
# add_executable(KungFuChessServer Server/main.cpp)
# target_include_directories(KungFuChessServer PRIVATE ${ALL_INCLUDE_DIRS})
# target_compile_definitions(KungFuChessServer PRIVATE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
# target_link_libraries(KungFuChessServer PRIVATE
#     ServerNetwork
#     ServerScheduler
#     ServerApplication
#     ServerAuth
#     ServerPersistence
#     ServerCore
#     Common
#     websocketpp::websocketpp
#     asio::asio
#     OpenSSL::SSL
#     OpenSSL::Crypto
#     nlohmann_json::nlohmann_json
# )

# # ---- טסטים — מתחברים לליבה, ל-Scheduler ול-Application ----
# enable_testing()

# file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS Server/tests/*.cpp)

# add_executable(server_tests ${TEST_SOURCES})
# target_link_libraries(server_tests PRIVATE kungfu_core ServerScheduler ServerApplication GTest::gtest GTest::gtest_main)

# include(GoogleTest)
# file(COPY ${CMAKE_SOURCE_DIR}/Server/assets DESTINATION ${CMAKE_BINARY_DIR})
# gtest_discover_tests(server_tests)
cmake_minimum_required(VERSION 3.16)
project(KungFuChessOnline CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# ----------------------------------------------------------------------------
# תלויות חיצוניות דרך vcpkg
# ----------------------------------------------------------------------------
find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ----------------------------------------------------------------------------
# ספריות header-only שנשמרות ידנית תחת third_party (לא דרך vcpkg)
# ----------------------------------------------------------------------------
set(NLOHMANN_JSON_DIR ${CMAKE_SOURCE_DIR}/third_party/nlohmann)

# websocketpp הוסר מרישום ה-ports הרשמי של vcpkg (הוסר יחד עם עדכון boost 1.87.0),
# אז הוא vendored ידנית כאן - בדיוק כמו nlohmann/json. ראו הוראות הורדה בהודעה הקודמת.
set(WEBSOCKETPP_DIR ${CMAKE_SOURCE_DIR}/third_party/websocketpp)

# ----------------------------------------------------------------------------
# איסוף אוטומטי של כל תיקיות ה-headers, בהתאם לסגנון ה-include הקיים בפרויקט
# (כל include הוא בשם קובץ בלבד, לא נתיב יחסי)
# הערה: נוח לפיתוח מהיר; כשהפרויקט יתייצב כדאי לשקול include dirs מפורשים.
# ----------------------------------------------------------------------------
file(GLOB_RECURSE ALL_HEADERS
    ${CMAKE_SOURCE_DIR}/Common/*.hpp
    ${CMAKE_SOURCE_DIR}/Server/*.hpp
    ${CMAKE_SOURCE_DIR}/Client/*.hpp
)

set(ALL_INCLUDE_DIRS "")
foreach(HEADER_FILE ${ALL_HEADERS})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND ALL_INCLUDE_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES ALL_INCLUDE_DIRS)

# ----------------------------------------------------------------------------
# ServerCore — הליבה הקיימת, ללא שינוי (Board/RuleEngine/GameEngine/RealTimeArbiter/...)
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Core/*.cpp
)
add_library(ServerCore STATIC ${SERVER_CORE_SOURCES})
target_include_directories(ServerCore PUBLIC ${ALL_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Common — DTO / Messages / Serialization
# ----------------------------------------------------------------------------
file(GLOB_RECURSE COMMON_SOURCES
    ${CMAKE_SOURCE_DIR}/Common/*.cpp
)
add_library(Common STATIC ${COMMON_SOURCES})
target_include_directories(Common PUBLIC ${ALL_INCLUDE_DIRS} ${NLOHMANN_JSON_DIR})
target_link_libraries(Common PUBLIC ServerCore nlohmann_json::nlohmann_json) # Messages.hpp תלוי ב-Position.hpp מה-Core

# ----------------------------------------------------------------------------
# ServerPersistence — Server/Persistence
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_PERSISTENCE_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Persistence/*.cpp
)
add_library(ServerPersistence STATIC ${SERVER_PERSISTENCE_SOURCES})
target_include_directories(ServerPersistence PUBLIC ${ALL_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# ServerAuth — Server/Auth (תלוי ב-OpenSSL וב-ServerPersistence)
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_AUTH_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Auth/*.cpp
)
add_library(ServerAuth STATIC ${SERVER_AUTH_SOURCES})
target_include_directories(ServerAuth PUBLIC ${ALL_INCLUDE_DIRS})
target_link_libraries(ServerAuth PUBLIC ServerPersistence OpenSSL::Crypto)

# ----------------------------------------------------------------------------
# ServerGameSession — Server/GameSession (תלוי ב-ServerCore, בלי לשנות אותו)
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_GAMESESSION_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/GameSession/*.cpp
)
add_library(ServerGameSession STATIC ${SERVER_GAMESESSION_SOURCES})
target_include_directories(ServerGameSession PUBLIC ${ALL_INCLUDE_DIRS})
target_link_libraries(ServerGameSession PUBLIC ServerCore Common)

# ----------------------------------------------------------------------------
# ServerScheduler — Server/Scheduler
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_SCHEDULER_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Scheduler/*.cpp
)
add_library(ServerScheduler STATIC ${SERVER_SCHEDULER_SOURCES})
target_include_directories(ServerScheduler PUBLIC ${ALL_INCLUDE_DIRS})
target_link_libraries(ServerScheduler PUBLIC ServerGameSession)

# ----------------------------------------------------------------------------
# ServerApplication — Server/Application (Command/Dispatcher; תלוי ב-ServerAuth)
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_APPLICATION_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Application/*.cpp
)
add_library(ServerApplication STATIC ${SERVER_APPLICATION_SOURCES})
target_include_directories(ServerApplication PUBLIC ${ALL_INCLUDE_DIRS} ${NLOHMANN_JSON_DIR})
target_link_libraries(ServerApplication PUBLIC ServerAuth Common nlohmann_json::nlohmann_json)

# ----------------------------------------------------------------------------
# ServerNetwork — Server/Network (WebSocketServer/ConnectionHandler/SessionRegistry)
# websocketpp הוא vendored (third_party) כי הוסר מ-vcpkg; asio מגיע מ-vcpkg כרגיל.
# ----------------------------------------------------------------------------
file(GLOB_RECURSE SERVER_NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/Network/*.cpp
)
add_library(ServerNetwork STATIC ${SERVER_NETWORK_SOURCES})
target_include_directories(ServerNetwork PUBLIC ${ALL_INCLUDE_DIRS} ${WEBSOCKETPP_DIR})
target_compile_definitions(ServerNetwork PUBLIC ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
target_link_libraries(ServerNetwork PUBLIC ServerApplication asio::asio)

# ----------------------------------------------------------------------------
# KungFuChessServer — הרצה אמיתית של השרת (Server/main.cpp)
# ----------------------------------------------------------------------------
add_executable(KungFuChessServer Server/main.cpp)
target_include_directories(KungFuChessServer PRIVATE ${ALL_INCLUDE_DIRS} ${WEBSOCKETPP_DIR})
target_compile_definitions(KungFuChessServer PRIVATE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
target_link_libraries(KungFuChessServer PRIVATE
    ServerNetwork
    ServerScheduler
    ServerApplication
    ServerAuth
    ServerPersistence
    ServerCore
    Common
)

# ----------------------------------------------------------------------------
# Client — קיים, ללא שינוי במבנה (משתמש ב-Core ישירות, לא עבר עדיין ל-GameGateway)
# ----------------------------------------------------------------------------
file(GLOB_RECURSE CLIENT_SOURCES
    ${CMAKE_SOURCE_DIR}/Client/*.cpp
)
add_executable(KungFuChessClient ${CLIENT_SOURCES})
target_include_directories(KungFuChessClient PRIVATE ${ALL_INCLUDE_DIRS} ${NLOHMANN_JSON_DIR})
target_link_libraries(KungFuChessClient PRIVATE ServerCore Common ${OpenCV_LIBS})

# ----------------------------------------------------------------------------
# server_tests — כל בדיקות ה-gtest תחת Server/tests
# ----------------------------------------------------------------------------
enable_testing()

file(GLOB_RECURSE SERVER_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/Server/tests/*.cpp
)

add_executable(server_tests ${SERVER_TEST_SOURCES})
target_include_directories(server_tests PRIVATE ${ALL_INCLUDE_DIRS} ${NLOHMANN_JSON_DIR} ${WEBSOCKETPP_DIR})
target_compile_definitions(server_tests PRIVATE ASIO_STANDALONE _WEBSOCKETPP_CPP11_STL_)
target_link_libraries(server_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    ServerNetwork
    ServerScheduler
    ServerApplication
    ServerAuth
    ServerPersistence
    ServerGameSession
    ServerCore
    Common
    asio::asio
)

include(GoogleTest)
gtest_discover_tests(server_tests)