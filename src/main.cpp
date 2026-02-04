#include <Arduino.h>
#include <PID_v1.h>
#include <Servo.h>
#include <HoltzLib.h>

DCMotor rightDriveMotor(9,8,10);
DCMotor leftDriveMotor(7, 6, 5);

UltrasonicSensor sensor(12,11);

double Kp = 1;
double Ki = 0;
double Kd = 0;

double input, output, setpoint;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  pid.SetMode(AUTOMATIC);
  setpoint = 100;

  Serial.begin(9600);
}

void loop()
{
  delay(500);
  input = sensor.getDistance(); 
  pid.Compute();
  Serial.print("Sensor: ");
  Serial.println(sensor.getDistance());
  Serial.print("In: ");
  Serial.println(input);
  Serial.print("Out: ");
  Serial.println(output);
  leftDriveMotor.drive(output);
  rightDriveMotor.drive(output);

  // leftDriveMotor.drive(255);
  // rightDriveMotor.drive(255);
}
