#include <ros/ros.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <iostream>
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/features/normal_3d.h>
#include <pcl/sample_consensus/method_types.h>  
#include <pcl/sample_consensus/model_types.h>   
#include <pcl/segmentation/sac_segmentation.h>

typedef pcl::PointXYZRGB PointT;

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

  pcl::PointCloud<PointT>::Ptr cloud( new pcl::PointCloud<PointT>() ), filtered_cloud( new pcl::PointCloud<PointT>() );

  sensor_msgs::PointCloud2 result;

   pcl::fromROSMsg(input,*cloud);

   pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients ());
   pcl::PointIndices::Ptr inliers (new pcl::PointIndices ()); 
   pcl::SACSegmentation<PointT> seg;
   seg.setInputCloud (cloud);
   seg.setOptimizeCoefficients(true);	
   seg.setMethodType(pcl::SAC_RANSAC); 
   seg.setModelType(pcl::SACMODEL_PARALLEL_LINE); 
   seg.setDistanceThreshold(0.1);                                                       // NEED TO UNDERSTAND TO TUNE THESE VALUES 
   seg.setAxis(Eigen::Vector3f(0,0,1)); // Vertical axis                                // NEED TO UNDERSTAND TO TUNE THESE VALUES 
   seg.setEpsAngle(0.174533);           //10 degrees                                    // NEED TO UNDERSTAND TO TUNE THESE VALUES 
   pcl::ExtractIndices<PointT> extract;

   // Obtain the plane inliers and coefficients
    seg.segment (*inliers, *coefficients);
    std::cerr << "Plane coefficients: " << *coefficients << std::endl;
 
   extract.setInputCloud (cloud);
   extract.setIndices (inliers);
   extract.setNegative (false);

  // std::cerr << "Model coefficients: " << coefficients->values[0] << " "
  //           << coefficients->values[1] << " "
  //           << coefficients->values[2] << " "
  //           << coefficients->values[3] << std::endl;
  // std::cerr << "Model inliers: " << inliers->indices.size() << std::endl;


  extract.filter (*filtered_cloud);

  pcl::toROSMsg(*filtered_cloud, result);

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
