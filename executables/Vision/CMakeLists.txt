add_executable(test testServer.cpp)
target_link_libraries(test PUBLIC ${OpenCV_LIBRARIES} visionLib modelEstimationLib)
target_compile_definitions(test PRIVATE DATA_PATH="${CMAKE_SOURCE_DIR}/DATA")
