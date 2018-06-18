#include "ros/ros.h"

#include "visualization_msgs/Marker.h"
#include "visualization_msgs/MarkerArray.h"

#include <tf/transform_broadcaster.h>
#include <tf/tf.h>
#include <tf2_msgs/TFMessage.h>

ros::Publisher g_marker_pub;

std::vector<std::string> tf_cache;

visualization_msgs::Marker frame_label(std::string frame_id){
    visualization_msgs::Marker marker;
    marker.header.frame_id = frame_id;
    marker.header.stamp = ros::Time::now();
    marker.ns = "marker_text";
    marker.id = 0;
    marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = 0.0;
    marker.pose.position.y = 0.0;
    marker.pose.position.z = 0.0;
    marker.scale.z = 0.015;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.text=frame_id;
    return marker;
}

void publishCallback(const ros::TimerEvent&)
{
    visualization_msgs::MarkerArray msg;
    {
        /// Add some instructions for the demo
        visualization_msgs::Marker marker;
        marker.header.frame_id = "/vrviz_base";
        marker.header.stamp = ros::Time::now();
        marker.ns = "marker_text";
        marker.id = 0;
        marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = 0.0;
        marker.pose.position.y = 0.7;
        marker.pose.position.z =-0.5;
        marker.scale.z = 0.020;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.text="USE WAND TRIGGER TO DRIVE";
        msg.markers.push_back(marker);
    }
    /// Send a text marker at the origin of every frame we know about
    for(int idx=0;idx<tf_cache.size();idx++){
        msg.markers.push_back(frame_label(tf_cache[idx]));
    }
//  {
//    visualization_msgs::Marker marker;
//    marker.header.frame_id = "/base_link";
//    marker.header.stamp = ros::Time::now();
//    marker.ns = "marker_test_mesh_color_change";
//    marker.id = 0;
//    marker.type = visualization_msgs::Marker::MESH_RESOURCE;
//    marker.action = visualization_msgs::Marker::ADD;
//    marker.pose.position.x = 0.0;
//    marker.pose.position.y = 0.0;
//    marker.pose.position.z = 1.0;
//    marker.pose.orientation.x = 0.0;
//    marker.pose.orientation.y = 0.0;
//    marker.pose.orientation.z = 0.0;
//    marker.pose.orientation.w = 1.0;
//    marker.pose.position.x = x_pos;
//    marker.scale.x = 1.0;
//    marker.scale.y = 1.0;
//    marker.scale.z = 1.0;
//    marker.color.r = float(counter % 255) / 255;
//    marker.color.g = float((counter*3) % 255) / 255;
//    marker.color.b = float((counter*10) % 255) / 255;
//    marker.color.a = 1.0;
//    marker.frame_locked = true;
//    marker.mesh_resource = "package://pr2_description/meshes/base_v0/base.dae";
//    marker.mesh_use_embedded_materials = false;
//    msg.markers.push_back(marker);
//  }
  g_marker_pub.publish(msg);

  static tf::TransformBroadcaster br;
  tf::Transform t;

  t.setOrigin(tf::Vector3(-0.1, 0.1, 0.6));
  t.setRotation(tf::Quaternion(0.0, 0.0, 0.0, 1.0));
  br.sendTransform(tf::StampedTransform(t, ros::Time::now(), "base_link", "my_tf_label"));

}

/*!
 * \brief add a frame to the tf cache
 * \param frame_id frame to add
 */
void add_tf_to_cache(std::string frame_id)
{
    if(frame_id.substr(0,10)=="vrviz_base"){return;}
    for(int idx=0;idx<tf_cache.size();idx++){
        if(frame_id==tf_cache[idx]){
            return;
        }
    }
    tf_cache.push_back(frame_id);
}

/*!
 * \brief tf_Callback for subscribing to TF messages
 *
 * We are just sniffing the /tf message and adding any frame_id's
 * we see to our cache.
 * \note that this will miss some frames, e.g. from robot_description
 *
 * \param msg Pointer to the message
 */
void tf_Callback(const tf2_msgs::TFMessage::ConstPtr& msg)
{
    for(int idx=0;idx<msg->transforms.size();idx++){
        add_tf_to_cache(msg->transforms[idx].header.frame_id);
        add_tf_to_cache(msg->transforms[idx].child_frame_id);
    }
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "marker_test");
  ros::NodeHandle n;

  ros::Subscriber tf_sub = n.subscribe("/tf", 1, tf_Callback);
  g_marker_pub = n.advertise<visualization_msgs::MarkerArray> ("marker_array", 0);
  ros::Timer publish_timer = n.createTimer(ros::Duration(1), publishCallback);

  tf::TransformBroadcaster tf_broadcaster;

  ros::Duration(0.1).sleep();

  ros::spin();
}

