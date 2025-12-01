cmake_minimum_required(VERSION 3.10)
project(TheGroupchat C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include directories
include_directories(include)

# Server source files
set(SERVER_SOURCES
    src/server/server_main.c
    src/server/server.c
    src/server/network.c
    src/server/config.c
    src/server/client_manager.c
)

set(CLIENT_SOURCES
    src/client/client.c
    src/client/client_main.c
    src/client/client_messaging.c
    src/client/client_network.c
    src/client/client_session.c
    src/client/client_ui.c)

# Header files
set(SERVER_HEADERS
    include/server/server.h
    include/server/network.h
    include/server/config.h
    include/server/client_manager.h
)
set(CLIENT_HEADERS
    include/client/client_messaging.h
    include/client/client_network.h
    include/client/client_session.h
    include/client/client_ui.h)

# Create server executable
add_executable(chat_server ${SERVER_SOURCES} ${SERVER_HEADERS})

# Create client executable
add_executable(chat_client ${CLIENT_SOURCES} ${CLIENT_HEADERS})