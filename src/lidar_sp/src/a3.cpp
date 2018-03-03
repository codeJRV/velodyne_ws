#include <ros/ros.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/filters/statistical_outlier_removal.h>

#include <iostream>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>

class cloud_filter
{

public:
  cloud_filter();
  void cloud_sub_pub(const sensor_msgs::PointCloud2 &input);

protected:
  ros::NodeHandle nh;
  ros::Subscriber pcl_sub;
  ros::Publisher pcl_pub;
};

cloud_filter::cloud_filter()
{
  pcl_sub = nh.subscribe("/statistical_outlier_removal_cloud", 10, &cloud_filter::cloud_sub_pub, this);
  pcl_pub = nh.advertise<sensor_msgs::PointCloud2>("upright_line_structure_detection_cloud", 1);
}

void cloud_filter::cloud_sub_pub(const sensor_msgs::PointCloud2 &input)
{
  pcl::PointCloud<pcl::PointXYZ> cloud, filtered_cloud;
  sensor_msgs::PointCloud2 result;

  pcl::fromROSMsg(input, cloud);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // std::cerr << "Point cloud data: " << cloud.points.size() << " points" << std::endl;

  // pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
  // pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
  // // Create the segmentation object
  // pcl::SACSegmentation<pcl::PointXYZRGB> seg;
  // // Optional
  // seg.setOptimizeCoefficients(true);
  // // Mandatory
  // seg.setModelType(pcl::SACMODEL_LINE);
  // seg.setMethodType(pcl::SAC_RANSAC);
  // seg.setDistanceThreshold(0.01);

  // seg.setInputCloud(cloud);
  // seg.segment(*inliers, *coefficients);

  // if (inliers->indices.size() == 0)
  // {
  //   PCL_ERROR("Could not estimate a linear model for the given dataset.");
  //   return (-1);
  // }

  // std::cerr << "Model coefficients: " << coefficients->values[0] << " "
  //           << coefficients->values[1] << " "
  //           << coefficients->values[2] << " "
  //           << coefficients->values[3] << std::endl;

  // std::cerr << "Model inliers: " << inliers->indices.size() << std::endl;



  // for (size_t i = 0; i < inliers->indices.size(); ++i)
  // { 
  //   filtered_cloud.points.push_back( cloud->points[inliers->indices[i]]);
  //   ++filtered_cloud.width;
  // }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  pcl::toROSMsg(filtered_cloud, result);

  result.header.stamp = input.header.stamp;
  result.header.frame_id = input.header.frame_id;

  pcl_pub.publish(result);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "a3");
  cloud_filter filter;
  ros::spin();
  return 0;
}
