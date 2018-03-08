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
#include <pcl/filters/voxel_grid.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/segmentation/extract_clusters.h>


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


typedef pcl::PointXYZRGB PointT;

}


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

  pcl::PointCloud<PointT>::Ptr cloud( new pcl::PointCloud<PointT>() ), filtered_cloud( new pcl::PointCloud<PointT>() ), cloud_f (new pcl::PointCloud<PointT>);

  sensor_msgs::PointCloud2 result;

   pcl::fromROSMsg(input,*cloud);

 std::cout << "PointCloud before filtering has: " << cloud->points.size () << " data points." << std::endl; //*

  // Create the filtering object: downsample the dataset using a leaf size of 1cm
  pcl::VoxelGrid<PointT> vg;
  pcl::PointCloud<PointT>::Ptr cloud_filtered (new pcl::PointCloud<PointT>);
  vg.setInputCloud (cloud);
  vg.setLeafSize (0.01f, 0.01f, 0.01f);
  vg.filter (*cloud_filtered);
  std::cout << "PointCloud after filtering has: " << cloud_filtered->points.size ()  << " data points." << std::endl; //*

  // Create the segmentation object for the Linear model and set all the parameters
  pcl::SACSegmentation<PointT> seg;
  pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
  pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  pcl::PointCloud<PointT>::Ptr cloud_plane (new pcl::PointCloud<PointT> ());
  pcl::PCDWriter writer;
  seg.setOptimizeCoefficients (true);
  seg.setMethodType(pcl::SAC_RANSAC); 
  seg.setModelType(pcl::SACMODEL_PARALLEL_LINE); 
  seg.setDistanceThreshold(0.1);                                                      //   TUNE THESE VALUES AS NEEDED
  seg.setAxis(Eigen::Vector3f(0,0,1)); // Vertical axis                               //   TUNE THESE VALUES AS NEEDED
  seg.setEpsAngle(0.01);                                                              //   TUNE THESE VALUES AS NEEDED
  seg.setMaxIterations (50);

  pcl::search::KdTree<PointT>::Ptr tree (new pcl::search::KdTree<PointT>);
  tree->setInputCloud (cloud_filtered);

  std::vector<pcl::PointIndices> cluster_indices;
  pcl::EuclideanClusterExtraction<PointT> ec;
  ec.setClusterTolerance (0.4); 
  ec.setMinClusterSize (100);
  ec.setMaxClusterSize (250000);
  ec.setSearchMethod (tree);
  ec.setInputCloud (cloud_filtered);
  ec.extract (cluster_indices);

  // int i=0, nr_points = (int) cloud_filtered->points.size ();
  // while (cloud_filtered->points.size () > 0.3 * nr_points)
  // {
  //   // Segment the largest linear component from the  cloud
  //   seg.setInputCloud (cloud_filtered);
  //   seg.segment (*inliers, *coefficients);
  //   if (inliers->indices.size () == 0)
  //   {
  //     std::cout << "Could not estimate a planar model for the given dataset." << std::endl;
  //     break;
  //   }

  //   // Extract the planar inliers from the input cloud
  //   pcl::ExtractIndices<pcl::PointXYZ> extract;
  //   extract.setInputCloud (cloud_filtered);
  //   extract.setIndices (inliers);
  //   extract.setNegative (false);

  //   // Get the points associated with the planar surface
  //   extract.filter (*cloud_plane);
  //   std::cout << "PointCloud representing the linear component: " << cloud_plane->points.size () << " data points." << std::endl;
  // }


  int j = 0;
  for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin (); it != cluster_indices.end (); ++it)
  {

    // pcl::PointCloud<PointT>::Ptr temp_cloud( new pcl::PointCloud<PointT>() );
    for (std::vector<int>::const_iterator pit = it->indices.begin (); pit != it->indices.end (); ++pit)
    {

      int color = j % N_COLORS;
      cloud_filtered->points[*pit].rgb = *reinterpret_cast<float*>(rainbow+color);
      filtered_cloud->points.push_back (cloud_filtered->points[*pit]); //*
    }

    j++;
  }

    filtered_cloud->width = filtered_cloud->points.size ();
    filtered_cloud->height = 1;
    filtered_cloud->is_dense = true;

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
