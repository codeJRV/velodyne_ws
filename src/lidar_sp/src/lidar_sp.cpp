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

//catkin_make -DCMAKE_BUILD_TYPE=Release

template <typename PointT>

class cloudHandler
{
public:

    cloudHandler()
    {
		cout<<"Cloudhandler started"<<endl;
		pcl_pub = nh.advertise<sensor_msgs::PointCloud2>("pcl_filtered", 1);
        pcl_sub = nh.subscribe<sensor_msgs::PointCloud2>("/velodyne_points", 10, &cloudHandler::cloud_callback, this);

    }

	void fromPCLPointCloud2ToVelodyneCloud(const pcl::PCLPointCloud2 &msg, pcl::PointCloud<PointT> &cloud1D, std::vector<pcl::PointCloud<PointT> > &cloudVector, unsigned int rings)
	{
		cloud1D.header = msg.header;
		cloud1D.width = msg.width;
		cloud1D.height = msg.height;
		cloud1D.is_dense = msg.is_dense == 1;
		uint32_t num_points = msg.width * msg.height;
		cloud1D.points.resize(num_points);
		uint8_t *cloud_data1 = reinterpret_cast<uint8_t *>(&cloud1D.points[0]);

		pcl::PointCloud<PointT> *cloudPerLaser = new pcl::PointCloud<PointT>[rings];
		uint8_t *cloud_data2[rings];

		unsigned int pointsCounter[rings] = {0};

		for (unsigned int i = 0; i < rings; ++i)
		{
			cloudPerLaser[i] = pcl::PointCloud<PointT>();
			cloudPerLaser[i].header = msg.header;
			cloudPerLaser[i].width = msg.width;
			cloudPerLaser[i].height = msg.height;
			cloudPerLaser[i].is_dense = msg.is_dense == 1;
			cloudPerLaser[i].points.resize(num_points);
			cloud_data2[i] = reinterpret_cast<uint8_t *>(&cloudPerLaser[i].points[0]);
		}

		for (uint32_t row = 0; row < msg.height; ++row)
		{
			const uint8_t *row_data = &msg.data[row * msg.row_step];

			for (uint32_t col = 0; col < msg.width; ++col)
			{
				const uint8_t *msg_data = row_data + col * msg.point_step;

				//float* x = (float*)msg_data;
				//float* y = (float*)(msg_data + 4);
				//float* z = (float*)(msg_data + 8);
				//float* i = (float*)(msg_data + 16);
				uint16_t *ring = (uint16_t *)(msg_data + 20);
				memcpy(cloud_data2[*ring], msg_data, 22);
				memcpy(cloud_data1, msg_data, 22);
				pointsCounter[*ring]++;
				cloud_data1 += sizeof(PointT);
				cloud_data2[*ring] += sizeof(PointT);
			}
		}

		cloudVector = std::vector<pcl::PointCloud<PointT> >(rings);

		for (unsigned int i = 0; i < rings; ++i)
		{
			cloudPerLaser[i].width = pointsCounter[i];
			cloudPerLaser[i].height = 1;
			cloudPerLaser[i].points.resize(pointsCounter[i]);
			cloudVector[i] = (cloudPerLaser[i]);
		}
	}

	void cloud_callback(const sensor_msgs::PointCloud2ConstPtr &cloud_msg)
	{

		pcl::PointCloud<pcl::PointXYZI>::Ptr cloud1D(new pcl::PointCloud<pcl::PointXYZI>);
		vector<pcl::PointCloud<pcl::PointXYZI> > cloudVector;
		pcl::PCLPointCloud2 pcl_pc2;
		pcl_conversions::toPCL(*cloud_msg, pcl_pc2);
		cloudHandler::fromPCLPointCloud2ToVelodyneCloud(pcl_pc2, *cloud1D, cloudVector, 14); //removing two channels here

		pcl::PointCloud <pcl::PointXYZ> cloud;
		sensor_msgs::PointCloud2 output;
  		//copyPointCloud (cloud, cloudVector);

		pcl::toROSMsg(cloud, output);
		output.header.frame_id = "point_cloud";
		pcl_pub.publish(output);

	}

protected:
    ros::NodeHandle nh;
    ros::Subscriber pcl_sub;
    ros::Publisher pcl_pub;

};




int main(int argc, char **argv)
{
	ros::init(argc, argv, "a1");
	ros::spin();
}