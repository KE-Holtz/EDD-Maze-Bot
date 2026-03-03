#include <math.h>
#include <Arduino.h>
#include <PID_v1.h>
#include <Servo.h>
#include <HoltzLib.h>

#define RIGHT_ANGLE -90
#define LEFT_ANGLE 90
#define FRONT_ANGLE 0

// How long to wait for the servo to arrive at it's target
#define SERVO_DELAY 500
// Minum distance for a measurement to be considered open
#define WALL_THRESHHOLD 10

DCMotor rightDriveMotor(9, 8, 10);
DCMotor leftDriveMotor(7, 6, 5);

UltrasonicSensor sensor(12, 11);
RotaryEncoder encoder(2);

Servo servo;

double Kp = 10;
double Ki = 0;
double Kd = 0;

double input, output, setpoint;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

enum State
{
  DRIVE_AHEAD,
  CHECK_RIGHT,
  CHECK_LEFT,
  TURN_RIGHT,
  TURN_LEFT,
  TURN_AROUND,
  WAIT,
};

State nextState = DRIVE_AHEAD;
State state = WAIT;
State previousState = WAIT;

void setup()
{
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-255, 255);
  encoder.begin();

  servo.attach(13);

  setpoint = 10;
}

void loop()
{
  previousState = state;
  state = nextState;

  if (state == DRIVE_AHEAD)
  {
    input = sensor.getDistance();
    pid.Compute();
  }

  switch (state)
  {
  case DRIVE_AHEAD:
    if (absError() < 2)
    {
      stop();
      nextState = CHECK_RIGHT;
    }
    else
    {
      driveToWall();
    }
    break;

  case CHECK_RIGHT:
    servo.write(RIGHT_ANGLE);
    delay(SERVO_DELAY);
    if (sensor.getDistance() > WALL_THRESHHOLD)
    {
      nextState = TURN_RIGHT;
    }
    else
    {
      nextState = CHECK_LEFT;
    }
    servo.write(FRONT_ANGLE);
    delay(SERVO_DELAY);
    break;

  case CHECK_LEFT:
    servo.write(LEFT_ANGLE);
    delay(SERVO_DELAY);
    if (sensor.getDistance() > WALL_THRESHHOLD)
    {
      nextState = TURN_LEFT;
    }
    else
    {
      nextState = TURN_AROUND;
    }
    servo.write(FRONT_ANGLE);
    delay(SERVO_DELAY);

    break;
  case TURN_RIGHT:
    // TODO: Implement turning right
    break;
  case TURN_LEFT:
    // TODO: Implement turning left
    break;
  case TURN_AROUND:
    // TODO: Implement turning around
    break;
  case WAIT:
    // This may do nothing
    break;
  }
}

int absError()
{
  return abs(input - setpoint);
}

void driveToWall()
{
  // If output is negative, drive backwards
  rightDriveMotor.drive(abs(output), output < 0);
  leftDriveMotor.drive(abs(output), output < 0);
}

void stop()
{
  rightDriveMotor.stop();
  leftDriveMotor.stop();
}