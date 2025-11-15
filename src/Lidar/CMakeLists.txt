add_library(LidarUtils src/LidarUtils.cpp)
target_link_libraries(LidarUtils ${catkin_LIBRARIES})

add_executable(...)
target_link_libraries(...)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        sensor_msgs
)
include_directories(${catkin_INCLUDE_DIRS})
