
#include "a1.h"
#include <velodyne_pointcloud/point_types.h>

/// @todo make sure these includes are really necessary
#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>


  /** types of output point and cloud */


  /** @brief Constructor. */
  RingCuts::RingCuts( )
  {
    // advertise output point cloud (before subscribing to input data)
    output_ =
      nh_.advertise<sensor_msgs::PointCloud2>("velodyne_cuts", 10);

    // subscribe to VelodyneScan packets
    input_ =
      nh_.subscribe("velodyne_points", 10,
                     &RingCuts::cutPoints, this,
                     ros::TransportHints().tcpNoDelay(true));
  }


  /** @brief Callback for Velodyne PointXYZRI messages. */
  void
    RingCuts::cutPoints(const VPointCloud::ConstPtr &inMsg)
  {
    if (output_.getNumSubscribers() == 0)         // no one listening?
      return;                                     // do nothing

    // allocate an PointXYZRGB message with same time and frame ID as
    // input data
    RGBPointCloud::Ptr outMsg(new RGBPointCloud());
    outMsg->header.stamp = inMsg->header.stamp;
    outMsg->header.frame_id = inMsg->header.frame_id;
    outMsg->height = 1;

    for (size_t i = 0; i < inMsg->points.size(); ++i)
      {
        RGBPoint p;
        p.x = inMsg->points[i].x;
        p.y = inMsg->points[i].y;
        p.z = inMsg->points[i].z;

        // color lasers with the rainbow array
        int color = inMsg->points[i].ring % N_COLORS;
        p.rgb = *reinterpret_cast<float*>(rainbow+color);

        if(inMsg->points[i].ring!= 0 && inMsg->points[i].ring!= 1 )
        {
          outMsg->points.push_back(p);
          ++outMsg->width;
        }

      }

    output_.publish(outMsg);
  }


int main(int argc, char** argv)
{
    ros::init(argc, argv, "a1");

    ROS_INFO("Started RingCuts Node");
    RingCuts newCut;


    ros::spin();

    return 0;
}


