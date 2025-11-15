
find_package(catkin REQUIRED COMPONENTS
        roscpp
        sensor_msgs
)

add_library(LidarUtils src/LidarUtils.cpp)
target_link_libraries(LidarUtils PUBLIC ${catkin_LIBRARIES})
target_include_directories(LidarUtils PUBLIC ${catkin_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
