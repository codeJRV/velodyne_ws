#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/PointField.h>
#include <pcl_conversions/pcl_conversions.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/common.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/PCLImage.h>
#include <pcl/point_cloud.h>
#include <pcl/exceptions.h>
#include <pcl/console/print.h>
#include <pcl/PCLPointField.h>
#include <pcl/PCLPointCloud2.h>

#include <iostream>
#include <ctime>
#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace pcl;

template <typename PointT>

void fromPCLPointCloud2To1DCloud(const pcl::PCLPointCloud2& msg, pcl::PointCloud<PointT>& cloud1D)
{
  cloud1D.header   = msg.header;
  cloud1D.width    = msg.width;
  cloud1D.height   = msg.height;
  cloud1D.is_dense = msg.is_dense == 1;
  uint32_t num_points = msg.width * msg.height;
  cloud1D.points.resize (num_points);
}

void cloud_callback (const sensor_msgs::PointCloud2ConstPtr& cloud_msg)
{
	
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1D(new pcl::PointCloud<pcl::PointXYZ>);
	vector< pcl::PointCloud<pcl::PointXYZ> > cloudVector;
	pcl::PCLPointCloud2 pcl_pc2;
  pcl_conversions::toPCL(*cloud_msg, pcl_pc2);
  fromPCLPointCloud2To1DCloud (pcl_pc2, *cloud1D);
  pcl::PCDWriter writer;
  writer.write<pcl::PointXYZ> ("cloud1D.pcd", *cloud1D, false);

}

int main (int argc, char** argv)
{
  ros::init (argc, argv, "a2_helper");
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe<sensor_msgs::PointCloud2> ("/velodyne_cuts", 10, cloud_callback);
  ros::spin ();
  return 0;
}