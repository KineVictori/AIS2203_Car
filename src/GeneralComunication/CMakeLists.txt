
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

set(SIMPLE_SOCKET_BUILD_TESTS OFF)
FetchContent_Declare(
        SimpleSocket
        GIT_REPOSITORY https://github.com/IslandRock1/SimpleSocket.git
        GIT_TAG "master"
)
FetchContent_MakeAvailable(SimpleSocket)

set(SOURCES
        GeneralComunication.cpp
        Data.cpp
)

add_library(generalComLib ${SOURCES})
target_link_libraries(generalComLib PUBLIC nlohmann_json simple_socket)
target_include_directories(generalComLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
