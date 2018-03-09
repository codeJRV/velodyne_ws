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

//TODO : Header Cleanup


namespace
{
// RGB color values
const int color_red = 0xff0000;
const int color_orange = 0xff8800;
const int color_yellow = 0xffff00;
const int color_green = 0x00ff00;
const int color_blue = 0x0000ff;
const int color_violet = 0xff00ff;

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

  pcl::PointCloud<PointT>::Ptr cloud(new pcl::PointCloud<PointT>()), filtered_cloud(new pcl::PointCloud<PointT>()), cloud_f(new pcl::PointCloud<PointT>);

  sensor_msgs::PointCloud2 result;

  pcl::fromROSMsg(input, *cloud);

  std::cout << "PointCloud before filtering has: " << cloud->points.size() << " data points." << std::endl; //*

  // Create the filtering object: downsample the dataset using a leaf size of 1cm
  pcl::VoxelGrid<PointT> vg;
  pcl::PointCloud<PointT>::Ptr cloud_filtered(new pcl::PointCloud<PointT>);
  vg.setInputCloud(cloud);
  vg.setLeafSize(0.01f, 0.01f, 0.01f);
  vg.filter(*cloud_filtered);
  std::cout << "PointCloud after filtering has: " << cloud_filtered->points.size() << " data points." << std::endl; //*

  // Perform a clustering and colour each cluster differently

  pcl::search::KdTree<PointT>::Ptr tree(new pcl::search::KdTree<PointT>);
  tree->setInputCloud(cloud_filtered);

  std::vector<pcl::PointIndices> cluster_indices;
  pcl::EuclideanClusterExtraction<PointT> ec;
  ec.setClusterTolerance(0.4);
  ec.setMinClusterSize(100);
  ec.setMaxClusterSize(250000);
  ec.setSearchMethod(tree);
  ec.setInputCloud(cloud_filtered);
  ec.extract(cluster_indices);

  // Process each cluster

  int j = 0;
  for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin(); it != cluster_indices.end(); ++it)
  {
    
    pcl::PointCloud<PointT>::Ptr temp_cloud(new pcl::PointCloud<PointT>());
    for (std::vector<int>::const_iterator pit = it->indices.begin(); pit != it->indices.end(); ++pit)
    {

      int color = j % N_COLORS;
      cloud_filtered->points[*pit].rgb = *reinterpret_cast<float *>(rainbow + color);
      temp_cloud->points.push_back(cloud_filtered->points[*pit]); //*
    }

    //Here we check the normals of each cloud cluster. Upright stuctures will have their z component 
    // of normals either close to +1 or -1

    pcl::NormalEstimation<PointT, pcl::Normal> ne;
    ne.setInputCloud(temp_cloud);
    pcl::search::KdTree<PointT>::Ptr tree(new pcl::search::KdTree<PointT>());
    ne.setSearchMethod(tree);
    pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);
    ne.setKSearch(6);
    ne.compute(*cloud_normals);

    int nok = 0;
    for (size_t i = 0; i < cloud_normals->points.size(); ++i)
    {
      if (cloud_normals->points[i].normal_z > -0.95 && cloud_normals->points[i].normal_z < 0.95 )
      { // Uncomment to debug
        //std::cout << "Normal"<<cloud_normals->points[i].normal_x<<"  "<<cloud_normals->points[i].normal_y<<"  "<<cloud_normals->points[i].normal_z<<" \n";
        nok++;
      }
    }

    float ratio  = ((float)nok / (float)cloud_normals->points.size());  //ratio of non vertical points to other points determine the 'uprightness' of a structure
    std::cout<<"CLUSTER : "<<j<<" ratio "<<ratio<<"\n\n\n\n\n";

    ///Push the temp_cloud points into the filtered cloud if the ratio of exceptions is < 0.09
    if (ratio<0.09)
    {
      for (size_t i = 0; i < temp_cloud->points.size(); ++i)
        filtered_cloud->points.push_back(temp_cloud->points[i]);
      std::cout << "Successfully pushed it \n";
    }

    j++;
  }

  filtered_cloud->width = filtered_cloud->points.size();
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
