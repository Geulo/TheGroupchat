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
    src/main.c
    src/server.c
    src/network.c
    src/config.c
    src/client_manager.c
)

# Header files
set(HEADERS
    include/server.h
    include/network.h
    include/config.h
    include/client_manager.h
)

# Create server executable
add_executable(chat_server ${SERVER_SOURCES} ${HEADERS})

# Create client executable
add_executable(chat_client src/client.c)