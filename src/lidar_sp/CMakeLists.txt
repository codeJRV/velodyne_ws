cmake_minimum_required(VERSION 2.8.3)
project(lidar_sp)

find_package(catkin REQUIRED COMPONENTS
pcl_conversions
pcl_ros
roscpp
sensor_msgs
velodyne_driver
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp executable
add_executable (lidar_sp src/lidar_sp.cpp)
target_link_libraries (lidar_sp ${catkin_LIBRARIES})

