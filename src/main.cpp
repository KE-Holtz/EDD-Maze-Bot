#include <math.h>
#include <Arduino.h>
#include <Servo.h>
#include <HoltzLib.h>

#define WHEEL_DIAMETER 6.3
#define ROBOT_DIAMETER 15.0

#define RIGHT_ANGLE 170
#define LEFT_ANGLE 0
#define FRONT_ANGLE 80

// How long to wait for the servo to arrive at it's target
#define SERVO_DELAY 500
// Minum distance for a measurement to be considered open
#define WALL_THRESHHOLD 15

DCMotor leftDriveMotor(9, 10, 11);
DCMotor rightDriveMotor(7, 8, 6);

UltrasonicSensor sensor(13, 12);
RotaryEncoder encoder(2, &leftDriveMotor);

Servo servo;

enum State
{
  CHECK_FRONT,
  DRIVE_AHEAD,
  TURN_RIGHT,
  TURN_LEFT
};

State nextState = CHECK_FRONT;
State state = CHECK_FRONT;

void setup()
{
  Serial.begin(9600);
  encoder.begin();
  servo.attach(5);

  leftDriveMotor.stop();
  rightDriveMotor.stop();
  servo.write(FRONT_ANGLE);
}

int distanceToWall;

void loop()
{
  // if (sensor.getDistance() > WALL_THRESHHOLD)
  // {
  //   leftDriveMotor.drive(255);
  //   rightDriveMotor.drive(255);
  //   delay(10);
  // } else {
  //   leftDriveMotor.drive(255);
  //   rightDriveMotor.drive(255, true);
  //   delay(10);
  // }
  
  state = nextState;
  Serial.println(encoder.getRobotTurnDegrees(WHEEL_DIAMETER, ROBOT_DIAMETER));
  switch (state)
  {
    case CHECK_FRONT:
      Serial.println("Checking front...");
      servo.write(FRONT_ANGLE);
      delay(SERVO_DELAY);
      distanceToWall = sensor.getDistance();
      if (distanceToWall <= WALL_THRESHHOLD){
        nextState = TURN_LEFT;
      }
      else{
        encoder.reset();
        servo.write(RIGHT_ANGLE);
        delay(SERVO_DELAY);
        nextState = DRIVE_AHEAD;
      }
    break;

    case DRIVE_AHEAD:
    Serial.println("Driving ahead...");
    rightDriveMotor.drive(100);
    leftDriveMotor.drive(100);
      if (sensor.getDistance() > WALL_THRESHHOLD)
      {
        nextState = TURN_RIGHT;
      } else if (encoder.getDistanceTraveled(WHEEL_DIAMETER) >= distanceToWall - WALL_THRESHHOLD)
      {
        encoder.reset();
        nextState = TURN_LEFT;
      }
    break; 
    
    case TURN_LEFT:
    Serial.println("Turning left...");
      leftDriveMotor.drive(100, true);
      rightDriveMotor.drive(100, false);
      if (encoder.getRobotTurnDegrees(WHEEL_DIAMETER, ROBOT_DIAMETER) <= -90)
      {
        encoder.reset();
        nextState = CHECK_FRONT;
      }
    break;

    case TURN_RIGHT:
    Serial.println("Turning right...");
      leftDriveMotor.drive(100, false);
      rightDriveMotor.drive(100, true);
      if (encoder.getRobotTurnDegrees(WHEEL_DIAMETER, ROBOT_DIAMETER) >= 90)
      {
        encoder.reset();
        nextState = CHECK_FRONT;
      }
    break; 
      
  }
}

void stop()
{
  rightDriveMotor.stop();
  leftDriveMotor.stop();
}