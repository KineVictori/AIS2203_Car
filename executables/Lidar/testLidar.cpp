#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include "LidarUtils.hpp"
#include "../../src/Lidar/LidarUtils.hpp"

LidarUtils lidar(0.0f, 40.0f); // center = 0 deg, width = 40 deg

void scanCallback(const sensor_msgs::LidarScan::ConstPtr& scan) {
    lidar.updateScan(*scan);

    float min_dist = lidar.minDistInSector();
    if (lidar.isObstacleWithin(0.7f)) {
        ROS_WARN_STREAM("Obstacle too close! Distance: " << min_dist << " m");
    } else {
        ROS_INFO_STREAM("Min distance in sector: " << min_dist << " m");
    }
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "Lidar");
    ros::NodeHandle nh;
    ros::Subscriber scan_sub = nh.subscribe("/scan", 10, &scanCallback);

    ros::spin();
    return 0;
}
