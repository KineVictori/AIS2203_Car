// Based on standard ROS C++ API and documentation here: https://wiki.ros.org/sensor_msgs

#include <algorithm>
#include <ros/ros.h>
#include "RobotSensors/RobotSensors.hpp"

int main(int argc, char **argv) {
    ros::init(argc, argv, "testRobotSensors");
    ros::NodeHandle n;

    RobotSensors sensors(n);

    ros::Rate loop_rate(10);
    while (ros::ok()) {
        ros::spinOnce();

        // Access latest data
        sensor_msgs::Imu imu = sensors.getLatestImu();
        sensor_msgs::LaserScan scan = sensors.getLatestScan();
        nav_msgs::Odometry odom = sensors.getLatestOdom();

        // bare tester, endre senere til faktisk bruk
        ROS_INFO_STREAM("IMU Acceleration X: " << imu.linear_acceleration.x);
        ROS_INFO_STREAM("LaserScan Min Range: " << (scan.ranges.empty() ? 0 : *std::min_element(scan.ranges.begin(), scan.ranges.end())));
        ROS_INFO_STREAM("Odometry X: " << odom.pose.pose.position.x);

        loop_rate.sleep();
    }
    return 0;
}