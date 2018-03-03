#ifndef _VELODYNE_POINTCLOUD_A1_H_
#define _VELODYNE_POINTCLOUD_A1_H_

#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <velodyne_pointcloud/point_types.h>

namespace
{
  // RGB color values
  const int color_red =       0xff0000;
  const int color_orange =    0xff8800;
  const int color_yellow =    0xffff00;
  const int color_green =     0x00ff00;
  const int color_blue =      0x0000ff;
  const int color_violet =    0xff00ff;


  const int N_COLORS = 6;
  int rainbow[N_COLORS] = {color_red, color_orange, color_yellow,
                           color_green, color_blue, color_violet};

  typedef pcl::PointXYZRGB RGBPoint;
  typedef pcl::PointCloud<RGBPoint> RGBPointCloud;
  typedef velodyne_pointcloud::PointXYZIR VPoint;
  typedef pcl::PointCloud<VPoint> VPointCloud;

}


  class RingCuts
  {
  public:

    RingCuts();
    ~RingCuts() {}

  private:

    void cutPoints(const VPointCloud::ConstPtr &inMsg);
    ros::NodeHandle nh_;
    ros::Subscriber input_;
    ros::Publisher output_;
  };


#endif // _VELODYNE_POINTCLOUD_A1_H_
