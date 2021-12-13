#include <ros/ros.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

int main (int argc, char **argv)
{
    ros::init(argc, argv, "my_tf2_listener");

    ros::NodeHandle node;

    ros::service::waitForService("spawn");
    ros::ServiceClient spawner = node.serviceClient<turtlesim::Spawn>("spawn");
    turtlesim::Spawn turtle;
    turtle.request.x = 0;
    turtle.request.y = 0;
    turtle.request.theta = 0;
    turtle.request.name = "turtle2";
    spawner.call(turtle);

    ros::ServiceClient spawner3 = node.serviceClient<turtlesim::Spawn>("spawn");
    turtlesim::Spawn turtle3;
    turtle3.request.x = 0;
    turtle3.request.y = 0;
    turtle3.request.theta = 0;
    turtle3.request.name = "turtle3";
    spawner3.call(turtle3);

    ros::Publisher rescuer = node.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 10);
    ros::Publisher lost = node.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 10);
    
    tf2_ros::Buffer tfBuffer;
    tf2_ros::TransformListener tfListener(tfBuffer);

    bool arrivedFlag = false;

    ros::Rate rate(10.0);
    while (node.ok())
    {
        if (!arrivedFlag)
        {
            /*Spasatel*/
            geometry_msgs::TransformStamped transformStamped;
            try
            {
                transformStamped = tfBuffer.lookupTransform("turtle2","turtle1", ros::Time(0));
            }
            catch(tf2::TransformException &ex)
            {
                ROS_WARN("%s", ex.what());
                ros::Duration(1.0).sleep();
                continue;
            }

            geometry_msgs::Twist vel_msg;

            vel_msg.angular.z = 4.0 * atan2(transformStamped.transform.translation.y, transformStamped.transform.translation.x);
            vel_msg.linear.x = 0.5 * sqrt(pow(transformStamped.transform.translation.x, 2) + pow(transformStamped.transform.translation.y, 2));

            if (vel_msg.linear.x < 0.1)
            {
                arrivedFlag = true;
            }
            
            rescuer.publish(vel_msg); 
        }else{
            /*Spasatel*/
            geometry_msgs::TransformStamped transformStamped1;
            try
            {
                transformStamped1 = tfBuffer.lookupTransform("turtle2","turtle3", ros::Time(0));
            }
            catch(tf2::TransformException &ex)
            {
                ROS_WARN("%s", ex.what());
                ros::Duration(1.0).sleep();
                continue;
            }

            geometry_msgs::Twist vel_msg1;

            vel_msg1.angular.z = 4.0 * atan2(transformStamped1.transform.translation.y, transformStamped1.transform.translation.x);
            vel_msg1.linear.x = 0.5 * sqrt(pow(transformStamped1.transform.translation.x, 2) + pow(transformStamped1.transform.translation.y, 2));
            
            rescuer.publish(vel_msg1);

            /*Poterashka*/
            geometry_msgs::TransformStamped transformStamped2;
            try
            {
                transformStamped2 = tfBuffer.lookupTransform("turtle1","turtle2", ros::Time(0));
            }
            catch(tf2::TransformException &ex)
            {
                ROS_WARN("%s", ex.what());
                ros::Duration(1.0).sleep();
                continue;
            }

            geometry_msgs::Twist vel_msg2;

            vel_msg2.angular.z = 4.0 * atan2(transformStamped2.transform.translation.y, transformStamped2.transform.translation.x);
            vel_msg2.linear.x = 0.5 * sqrt(pow(transformStamped2.transform.translation.x, 2) + pow(transformStamped2.transform.translation.y, 2));
            
            lost.publish(vel_msg2);
        } 

        rate.sleep();   
    }
    return 0;
};