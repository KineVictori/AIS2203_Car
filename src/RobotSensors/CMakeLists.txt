
find_package(catkin QUIET COMPONENTS
        roscpp
        sensor_msgs
        nav_msgs
)

if(catkin_FOUND)
    message("Found catkin")

    add_library(robotSensorsLib RobotSensors.cpp)
    target_link_libraries(robotSensorsLib PUBLIC ${catkin_LIBRARIES})
    target_include_directories(robotSensorsLib PUBLIC ${catkin_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

else()
    message(WARNING "Could not find catkin")
endif()

set(SHARED_SOURCES
        Vision.cpp
)