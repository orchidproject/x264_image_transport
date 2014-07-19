#include <string>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

/**
 * Publishes two different images alternatively in a loop.
 */
int main(int argc, char** argv)
{
  //***************************************************************************
  //	Initialise this ROS Node
  //***************************************************************************
  ros::init(argc, argv, "x264_test_publisher");
  ros::NodeHandle nh("~");

  //***************************************************************************
  //	Read ROS Pameters:
  //    refreshRate - rate at which to alternate images
  //    topic_name - topic to publish to
  //    image1 - filename of first image
  //    image2 - filename of second image
  //***************************************************************************
  int refreshRate;
  nh.param<int>("refresh_rate", refreshRate, 15);

  std::string topicName;
  nh.param<std::string>("topic_name", topicName, "image");

  std::string image1;
  nh.param<std::string>("image1", image1,"");
  ROS_INFO("First image file is: %s",image1.c_str());

  std::string image2;
  nh.param<std::string>("image2", image2,"");
  ROS_INFO("Second image file is: %s",image2.c_str());

  //***************************************************************************
  //  Advertise image topic
  //***************************************************************************
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise(topicName, 10);

  //***************************************************************************
  //  Load images from file (into CvBridge::CvImage structure - ready for
  //  for conversion to ros message)
  //***************************************************************************
  cv_bridge::CvImage buff1;
  buff1.header.frame_id = "image";
  buff1.encoding = "bgr8";
  buff1.image = cv::imread(image1);

  cv_bridge::CvImage buff2;
  buff2.header.frame_id = "image";
  buff2.encoding = "bgr8";
  buff2.image = cv::imread(image2);

  //***************************************************************************
  //	Copy images into ROS image messages
  //***************************************************************************
  sensor_msgs::Image msg1;
  sensor_msgs::Image msg2;
  buff1.toImageMsg(msg1);
  buff2.toImageMsg(msg2);
  
  //***************************************************************************
  //	Iteratively publish each image in turn
  //***************************************************************************
  ros::Rate loop_rate(refreshRate);
  bool selector = true;
  while (nh.ok()) {
    if(selector)
    {
       msg1.header.stamp = ros::Time::now();
       pub.publish(msg1);
    }
    else
    {
       msg2.header.stamp = ros::Time::now();
       pub.publish(msg2);
    }
    selector = !selector;
    ros::spinOnce();
    loop_rate.sleep();
  }

} // main


