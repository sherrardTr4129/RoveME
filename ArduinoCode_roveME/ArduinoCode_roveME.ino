#include <Servo.h>

/*
 * rosserial PubSub Example
 * Prints "hello world!" and toggles led
 */

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/Twist.h>

Servo TalonR; 
Servo TalonL;

ros::NodeHandle  nh;


void cmdVelCb(const geometry_msgs::Twist& msg) {
  float x = msg.linear.x; // m/s
  float th = msg.angular.z; // rad/s
  float spd_left, spd_right;
  float wheelTrack = 0.319186; //meters
  int talonRSpeed; 
  int talonLSpeed;

  if (x == 0 && th == 0) {
    moving = 0;
    TalonR.write(96);
    TalonL.write(96);
    return;
  }
  /* Indicate that we are moving */
  moving = 1;
  if (x == 0) {
    // Turn in place
    spd_right = th * wheelTrack / 2.0;
    spd_left = -spd_right;
  }
  else if (th == 0) {
    // Pure forward/backward motion
    spd_left = spd_right = x;
  }
  else {
    // Rotation about a point in space
    spd_left = x - th * wheelTrack / 2.0;
    spd_right = x + th * wheelTrack / 2.0;
  }
  if(spd_right < 0)
  {
   talonRSpeed = map(spd_right, -2.0, 0, 90, 180);
  }
  else if (spd_right > 0)
  {
    talonRSpeed = map(spd_right, 0, 2.0, 90, 0);
  }
  if(spd_left < 0)
  {
    talonLSpeed = map(spd_left, -2.0, 0, 0, 90);
  }
  else if(spd_left > 0)
  {
    talonLSpeed = map(spd_left, 0, 2.0, 90, 180);
  }
  TalonR.write(talonRSpeed);
  TalonL.write(talonLSpeed);
}

ros::Subscriber<std_msgs::Empty> motor_sub("/cmd_vel", messageCb );



void setup()
{
  TalonR.attach(6);
  TalonL.attach(5):
  nh.initNode();
  nh.advertise(chatter);
  nh.Subcribe(motor_sub);

}

void loop()
{
  nh.spinOnce();
  delay(500);
}

