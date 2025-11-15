add_executable(test testServer.cpp)
target_link_libraries(test PUBLIC ${OpenCV_LIBRARIES} visionLib)

find_package(Threads REQUIRED)
target_link_libraries(test PRIVATE Threads::Threads)
