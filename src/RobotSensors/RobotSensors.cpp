// Based on standard ROS C++ API and documentation here: https://wiki.ros.org/sensor_msgs
// Dobbeltsjekk bruken min av & og *, ikke 100% kontroll der,
// brukte som oftest det som kom opp som forslag

#include "../../include/RobotSensors/RobotSensors.hpp"

RobotSensors::RobotSensors(ros::NodeHandle &nh) {
    imu_sub_ = nh.subscribe("/imu/data", 10, &RobotSensors::imuCallback, this);
    scan_sub_ = nh.subscribe("/scan", 10, &RobotSensors::scanCallback, this);
    odom_sub_ = nh.subscribe("/odom", 1, &RobotSensors::odomCallback, this);
}

void RobotSensors::imuCallback(const sensor_msgs::Imu::ConstPtr &msg) {
    latest_imu_ = *msg;
}

void RobotSensors::scanCallback(const sensor_msgs::LaserScan::ConstPtr &msg) {
    latest_scan_ = *msg;
}

void RobotSensors::odomCallback(const nav_msgs::Odometry::ConstPtr &msg) {
    latest_odom_ = *msg;
}

sensor_msgs::Imu RobotSensors::getLatestImu() const {
    return latest_imu_;
}

sensor_msgs::LaserScan RobotSensors::getLatestScan() const {
    return latest_scan_;
}

nav_msgs::Odometry RobotSensors::getLatestOdom() const {
    return latest_odom_;
}