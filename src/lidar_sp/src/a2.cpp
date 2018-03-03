#include<ros/ros.h>
#include<pcl/point_cloud.h>
#include<pcl_conversions/pcl_conversions.h>
#include<sensor_msgs/PointCloud2.h>
#include<pcl/filters/statistical_outlier_removal.h>

class cloud_filter{

public:
  cloud_filter();
  void cloud_sub_pub(const sensor_msgs::PointCloud2& input);

protected:
  ros::NodeHandle nh;
  ros::Subscriber pcl_sub;
  ros::Publisher pcl_pub;
};

cloud_filter::cloud_filter()
{
pcl_sub = nh.subscribe("/two_channel_removal_cloud",10,&cloud_filter::cloud_sub_pub,this);
pcl_pub = nh.advertise<sensor_msgs::PointCloud2>("statistical_outlier_removal_cloud",1);
}

void cloud_filter::cloud_sub_pub(const sensor_msgs::PointCloud2 & input)
{
pcl::PointCloud<pcl::PointXYZRGB> cloud, filtered_cloud;
sensor_msgs::PointCloud2 result;


pcl::fromROSMsg(input,cloud);
pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> statFilter;
statFilter.setInputCloud(cloud.makeShared());
statFilter.setMeanK(50);
statFilter.setStddevMulThresh(0.2);
statFilter.filter(filtered_cloud);
pcl::toROSMsg(filtered_cloud,result);

result.header.stamp = input.header.stamp;
result.header.frame_id = input.header.frame_id;

pcl_pub.publish(result);

}

int main(int argc,char ** argv)
{
ros::init(argc,argv,"a2");
cloud_filter filter;
ros::spin();
return 0;

}