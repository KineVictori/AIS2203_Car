
#include "lidarUtils.hpp"

#include <cmath>
#include <sensor_msgs/LaserScan.h>


LidarUtils::LidarUtils(float center_angle_deg, float sector_width_deg) : center_angle_deg(center_angle_deg), sector_width_deg(sector_width_deg) {

}

void LidarUtils::updateScan(const sensor_msgs::LaserScan &scan) {
    scan_ = scan;
}

void LidarUtils::setSector(float center_angle_deg, float sector_width_deg) {
    center_angle_deg = center_angle_deg;
    sector_width_deg = sector_width_deg;
}

float LidarUtils::minDistInSector() const {
    if (scan_.ranges.empty())
        return scan_.range_max;

    float min_dist = scan_.range_max;
    float angle_min_deg = scan_.angle_min * 180.0f / M_PI;
    float angle_inc_deg = scan_.angle_increment * 180 / M_PI;

    int total = scan_.ranges.size();
    float half_sector = sector_width_deg / 2.0f;

    for (int i = 0; i < total; i++) {
        float angle_deg = angle_min_deg + i * angle_inc_deg;
        if (std::fabs(angle_deg - center_angle_deg) < half_sector) {
            float r = scan_.ranges[i];
            if (r > scan_.range_min && r < min_dist)
                min_dist = r;
        }
    }
    return min_dist;
}

bool LidarUtils::isObstacleWithin(float max_distance) const {
    return minDistInSector() < max_distance;
}

