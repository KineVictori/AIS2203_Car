
find_package(catkin QUIET COMPONENTS
        roscpp
        sensor_msgs
)

if(catkin_FOUND)
    message("Found catkin")
    add_library(LidarUtils LidarUtils.cpp)
    target_link_libraries(LidarUtils PUBLIC ${catkin_LIBRARIES})
    target_include_directories(LidarUtils PUBLIC ${catkin_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
else()
    message(WARNING "Could not find catkin")
endif()