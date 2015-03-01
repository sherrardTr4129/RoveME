
#include <ros.h>
#include <ros/time.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobLeft(3, 8);
Encoder knobRight(2, 9);

long _PreviousLeftEncoderCounts = 0;
long _PreviousRightEncoderCounts = 0;
double DistancePerCount = ((3.14159265 * 0.106) / (2048*26.9));

double x;
double y;
double th;
double theta;
double vx;
double vy;
double vth;
double deltaLeft;
double deltaRight;



double margin = .01;
double posx;
double posy;

double SinPrev;
double CosPrev;

double WHEELBASE = .4318;
ros::NodeHandle nh;
tf::TransformBroadcaster odom_broadcaster; 
geometry_msgs::TransformStamped odom_trans;

void setup()
{
  nh.initNode();
  odom_broadcaster.init(nh);
}

void loop()
{

  deltaLeft =  knobLeft.read() - _PreviousLeftEncoderCounts;
  deltaRight = -1*knobRight.read() - _PreviousRightEncoderCounts;
  
  vx = deltaLeft * DistancePerCount; // (current_time_encoder - last_time_encoder).toSec();
  vy = deltaRight * DistancePerCount; // (current_time_encoder - last_time_encoder).toSec();


  _PreviousLeftEncoderCounts = knobLeft.read();
  _PreviousRightEncoderCounts = -1*knobRight.read();
  
     if(abs(vy-vx) < margin && ((vy-vx) != 0))
     {
      posy += ((vx+vy)/2) * SinPrev;
      posx += ((vx+vy)/2) * CosPrev;   
     }
     else if(abs(vy-vx) > margin && ((vy-vx) != 0))
     {
      double dtheta = (vy-vx)/WHEELBASE;
      theta =  (theta+dtheta);
      CosPrev=cos(theta); // for the next cycle
      SinPrev=sin(theta);
     }


  //since all odometry is 6DOF we'll need a quaternion created from yaw
  geometry_msgs::Quaternion odom_quat = tf::createQuaternionFromYaw(theta);


  odom_trans.header.stamp = nh.now();
  odom_trans.header.frame_id = "/odom";
  odom_trans.child_frame_id = "/base_link";

  odom_trans.transform.translation.x = posx;
  odom_trans.transform.translation.y = posy;
  odom_trans.transform.translation.z = 0.0;
  odom_trans.transform.rotation = odom_quat;

  //send the transform
  odom_broadcaster.sendTransform(odom_trans);

  nh.spinOnce();
  delay(100);

}




