add_executable(test testServer.cpp)
target_link_libraries(test PUBLIC ${OpenCV_LIBRARIES} visionLib modelEstimationLib)

