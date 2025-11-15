
include(FetchContent)
set(SIMPLE_SOCKET_BUILD_TESTS OFF)
FetchContent_Declare(
        SimpleSocket
        GIT_REPOSITORY https://github.com/IslandRock1/SimpleSocket.git
        GIT_TAG "master"
)
FetchContent_MakeAvailable(SimpleSocket)

set(SHARED_SOURCES
        Vision.cpp
)

add_library(visionLib ${SHARED_SOURCES})
target_link_libraries(visionLib PUBLIC ${OpenCV_LIBRARIES} simple_socket)
target_include_directories(visionLib PUBLIC ${CMAKE_SOURCE_DIR}/include)