// Based on standard ROS C++ API and documentation here: https://wiki.ros.org/sensor_msgs

#ifndef ROBOTSENSORS_HPP
#define ROBOTSENSORS_HPP

#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/LaserScan.h>
#include <nav_msgs/Odometry.h>


class RobotSensors {

public:
    RobotSensors(ros::NodeHandle& nh);

    // Methods to access the latest sensor data
    sensor_msgs::Imu getLatestImu() const;
    sensor_msgs::LaserScan getLatestScan() const;
    nav_msgs::Odometry getLatestOdom() const;

private:
    void imuCallback(const sensor_msgs::Imu::ConstPtr &msg);
    void scanCallback(const sensor_msgs::LaserScan::ConstPtr &msg);
    void odomCallback(const nav_msgs::Odometry::ConstPtr &msg);

    sensor_msgs::Imu latest_imu_;
    sensor_msgs::LaserScan latest_scan_;
    nav_msgs::Odometry latest_odom_;

    ros::Subscriber imu_sub_;
    ros::Subscriber scan_sub_;
    ros::Subscriber odom_sub_;
};



#endif //ROBOTSENSORS_HPP
