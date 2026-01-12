#include <Arduino.h>
#include <PID_v1.h>

struct DCMotor {
  uint8_t in1;
  uint8_t in2;
  uint8_t power;
  boolean reversed;
};

struct UltrasonicSensor {
  uint8_t trig;
  uint8_t echo;
};

DCMotor rightDriveMotor;
DCMotor leftDriveMotor;

UltrasonicSensor sensor;

/**
 * Creates a DCMotor struct, initializes all pins. Using DCMotor and the other utility methods
 * avoids directly setting values to the pins directly.
 * @param in1 The pin of the arduino that is connected to in1/in3 on the motor controller.
 * @param in2 The pin of the arduino that is connected to in2/in4 on the motor controller.
 * @param power The pin of the arduino that is connected to ENA/ENB on the motor controller.
 * This will use PWM, so use one of the pins with the ~ symbol on the arduino.
 * @param reversed Whether or not to reverse the motor. This is useful when two motors are physically
 * facing directions so they can be sent the same direction in code. 
*/
DCMotor DCMotorInit(uint8_t in1, uint8_t in2, uint8_t power, boolean reversed){
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(power, OUTPUT);
  DCMotor motor;
  motor.in1 = in1;
  motor.in2 = in2;
  motor.power = power;
  motor.reversed = reversed;
  return motor;
}

/**
 * Sets the power and direction of a DCMotor.
 * 
 * @param motor The motor that will be driven.
 * 
 * @param power The power to drive the motor with.
 * Ranges from 0 to 255, where 0 is stopped and 255 is max. 
 * If you want to use a different range of numbers, e.g., 0-100, 
 * you can use the built in map function:
 * map(valueToMap, fromLow, fromHigh, toLow, toHigh);
 * For example, to run at 90% power:
 * setMotor(motor, map(90, 0, 100, 0, 255), false);
 * map(90, 0, 100, 0, 255);
 * 
 * @param reverse Should the motor be driven in the reverse direction or not?
 */
void setMotor(DCMotor motor, int power, boolean reverse){
  if((motor.reversed && reverse) || (!motor.reversed && !reverse)){
    digitalWrite(motor.in1, HIGH);
    digitalWrite(motor.in2, LOW);
  } else if (motor.reversed || reverse){
    digitalWrite(motor.in1, LOW);
    digitalWrite(motor.in2, HIGH); 
  }

  analogWrite(motor.power, power);
}

/**
 * Creates an Ultrasonic Sensor struct. This isn't as important as the DCMotor, but still useful.
 * @param trig The pin on the arduino connected to Trig.
 * @param echo The pin on the arduino connected to Echo.
 */
UltrasonicSensor UltrasonicSensorInit(uint8_t trig, uint8_t echo){
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  UltrasonicSensor sensor;
  sensor.echo = echo;
  sensor.trig = trig;
  return sensor;
}

float getDistance(UltrasonicSensor sensor){
  digitalWrite(sensor.trig, LOW);
  delayMicroseconds(2);
  digitalWrite(sensor.trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor.trig, LOW);

  return (pulseIn(sensor.echo, HIGH) * 0.0343) / 2;
}

void setup() {
  // rightDriveMotor = DCMotorInit(13, 12, 11, false);
  // leftDriveMotor = DCMotorInit(8, 7, 6, true);

  sensor = UltrasonicSensorInit(10,9);
  Serial.begin(9600);
}

void loop() {
  delay(10);
  Serial.println(getDistance(sensor));
}