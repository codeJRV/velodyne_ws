# Code for the Velodyne Challenge

PROBLEM STATEMENT : below please see the quiz. Most of our master level candidates complete it in 24 hours.

====
The Algorithm & DSP team at Velodyne Lab is developing various algorithms to enable our product to be a smart sensor. 

Please implement a lidar signal processing software in C++ in ROS that contains at least the following components.
* (a). three runnable ROS nodes: 
  * node (a1) removing 2 channels of signals from the 16 channels of Velodyne VLP16 
  * node (a2) removing noise (e.g., isolating points) in the remaining point cloud 
  * node (a3) detecting upright line structures in the point cloud and visualizing in different colors
  
* (b). a demo program
* (c). necessary scripts
* (d). documentation

Feel free to use any library you think necessary. You may find the following links useful:

### Download dataset:
* Download the dataset from [here](https://velodyne-my.sharepoint.com/personal/ykim_velodyne_com/_layouts/15/guestaccess.aspx?docid=1bf125a78846d4bc8b99c05b8208f32cc&authkey=AZr5rkIbs6GziwT5oAoJBiA).

Velodyne_driver in ROS: (http://wiki.ros.org/velodyne_driver)
 
ROS bag: (http://wiki.ros.org/rosbag/Tutorials/Recording%20and%20playing%20back%20data)


# Solutions to the Velodyne Challenge

#  View in RVIZ

In this repository you will find a simple launch file to play a Velodyne rosbag file and visualize the result in RViz.

## Instructions:

### Install ROS:

Follow instructions you find in this <a href="http://wiki.ros.org/ROS/Installation" target="_parent">page</a>.

* Point 1.2 : choose instructions for your version of Ubuntu.

* Point 1.4 : Desktop-Full Install: (Recommended).

* When you configure the ROS Environment, choose catkin.


Once we have installed ROS with all the dependencies : PCL, Velodyne, etc and we have created our workspace.

* Open the file bashrc:
	` gedit ~/.bashrc`
if you want to avoid to type the following command every time, add in the end of the file :	
	
  ``` 
  export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:/path_to_catkin_ws/catkin_ws/src
  source /opt/ros/indigo/setup.bash
  source /path_to_catkin_ws/catkin_ws/devel/setup.bash
  ```
Remember to change `/path_to_catkin_ws` with your path and change `indigo` if you are using another version of ROS.

Close the terminal an open a new one.

I used Ubuntu 16.04 and ROS Kinetic.

### Clone the repo:
* Go in the src folder of your catkin workspace in catkin_ws/src via terminal   
* Clone the repository typing:   
  `$ git clone https://github.com/codeJRV/velodyne_ws.git`   
* Go in your `velodyne_ws` and do `catkin_make`:   
  `$ cd ~/velodyne_ws`   
  `$ catkin_make`   
* Now source the setup.bash;   
  `$ source devel/setup.bash`   
* Check if ROS is able to find the package:  
  `$ roscd velodyne_visualizer`  


### Visualize Lidar, camera and Laser using RViz:

* Task 0 : Visualizing the given data 

Launch `demo_rosbag_rviz.launch` optionally setting the path to the rosbag file you want to use:   
 ```
 $ roslaunch velodyne_visualizer demo_rosbag_rviz.launch
 ```
 
* Task 1 : Visualizing the given data with node (a1) removing 2 channels of signals from the 16 channels of Velodyne VLP16 
 
 Launch `demo_a1.launch` optionally setting the path to the rosbag file you want to use:   
 ```
 $ roslaunch velodyne_visualizer demo_a1.launch
 
 ```
 * Task 2 : Visualizing the given data with node (a2) removing noise (e.g., isolating points) in the remaining point cloud from node (a1)
 
 Launch `demo_a2.launch` optionally setting the path to the rosbag file you want to use:   
 ```
 $ roslaunch velodyne_visualizer demo_a2.launch
 ```
 
 * Task 3 : Visualizing the given data with node (a3) detecting upright line structures in the point cloud and visualizing in different colors from node(a2)
 
 Launch `demo_a3.launch` optionally setting the path to the rosbag file you want to use:   
 ```
 $ roslaunch velodyne_visualizer demo_a3.launch
 ```

 
#### Explanation launch file
The launch file `demo_*.launch` is doing the following:

 * Running task nodes from the Lidar Signal Processing Package 'lidar_sp '   
```
<node name="a3" pkg="lidar_sp" type="a*" />
``
* Publishing the transformation between the link 'base_link' and 'velodyne'    
```
<node pkg="tf2_ros" type="static_transform_publisher" name="link1_broadcaster" args="1.9 0 1.6 0 0 0 1 base_link velodyne" />
```

* Playing in loop the rosbag file   
```
<arg name="rosbag_file" default="$(find velodyne_visualizer)/bagfiles/VLP16_video_column.bag"  />

```

* Opening RViz with the setting file 'display.rviz'   
```
<node name="rviz" pkg="rviz" type="rviz" args="-f velodyne -d $(find velodyne_visualizer)/display.rviz" />
```

