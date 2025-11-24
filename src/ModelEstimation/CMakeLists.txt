
find_package(Threads REQUIRED)

set(SHARED_SOURCES
        ModelEstimation.cpp
        inference.cpp
)

add_library(modelEstimationLib ${SHARED_SOURCES})
target_link_libraries(modelEstimationLib PUBLIC ${OpenCV_LIBRARIES})
target_link_libraries(modelEstimationLib PRIVATE Threads::Threads)
target_include_directories(modelEstimationLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(modelEstimationLib PRIVATE DATA_PATH="${CMAKE_SOURCE_DIR}/DATA")