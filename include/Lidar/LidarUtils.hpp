
#ifndef LIDAR_HPP
#define LIDAR_HPP



class LidarUtils {

    public:
    // Set sector center/width here (degrees)
    LidarUtils(float center_angle_deg = 0.0f, float sector_width_deg = 40.0f);

    // Update the class with latest scan
    void updateScan(const sensor_msgs::LaserScan& scan);

    // Get minimun distance in the sector configured
    float minDistInSector() const;

    // Check if any obstacle within max_distance in the sector
    bool isObstacleWithin(float max_distance) const;

    // Set sector fpr analysis
    void setSector(float center_angle_deg, float sector_width_deg);

    private:
    sensor_msgs::LaserScan scan_;
    float center_angle_deg;
    float sector_width_deg;
};


#endif //LIDAR_HPP
