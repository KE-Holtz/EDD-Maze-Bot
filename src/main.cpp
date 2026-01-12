#include <Arduino.h>
#include <PID_v1.h>

class DCMotor
{
private:
  uint8_t in1;
  uint8_t in2;
  uint8_t powerPin;
  boolean reversed;

public:
  /**
   * Creates a DCMotor and initializes all pins. Using DCMotor and the other utility methods
   * avoids directly setting values to the pins directly.
   * @param in1 The pin of the arduino that is connected to in1/in3 on the motor controller.
   * @param in2 The pin of the arduino that is connected to in2/in4 on the motor controller.
   * @param power The pin of the arduino that is connected to ENA/ENB on the motor controller.
   * This will use PWM, so use one of the pins with the ~ symbol on the arduino.
   * @param reversed Whether or not to reverse the motor. This is useful when two motors are physically
   * facing directions so they can be sent the same direction in code.
   */
  DCMotor(uint8_t in1, uint8_t in2, uint8_t powerPin, boolean reversed)
  {
    this->in1 = in1;
    this->in2 = in2;
    this->powerPin = powerPin;
    this->reversed = reversed;
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(powerPin, OUTPUT);
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
   */
  void drive(int power, boolean reverse)
  {
    if ((reversed && reverse) || (!reversed && !reverse))
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    else if (reversed || reverse)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }

    analogWrite(powerPin, power);
  }
};

class UltrasonicSensor
{
private:
  uint8_t trig;
  uint8_t echo;

public:
  /**
   * Creates an Ultrasonic Sensor, helpful for easily calculating distance.
   * @param trig The pin on the arduino connected to Trig.
   * @param echo The pin on the arduino connected to Echo.
   */
  UltrasonicSensor(uint8_t trig, uint8_t echo)
  {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    this->echo = echo;
    this->trig = trig;
  }

  float getDistance(){
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    return (pulseIn(echo, HIGH) * 0.0343) / 2.0;
  }
};
//========== End Library Code =============

DCMotor rightDriveMotor;
DCMotor leftDriveMotor;

UltrasonicSensor sensor;

double Kp = 1;
double Ki = 0;
double Kd = 0;

double input, output, setpoint;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  rightDriveMotor = DCMotor(13, 12, 11, false);
  leftDriveMotor = DCMotor(8, 7, 6, true);

  sensor = UltrasonicSensor(10, 9);

  pid.SetMode(AUTOMATIC);
  setpoint = 500;

  Serial.begin(9600);
}

void loop()
{
  delay(1000);
  input = sensor.getDistance(); 
  pid.Compute();
  Serial.print("Sensor: ");
  Serial.println(sensor.getDistance());
  Serial.print("In: ");
  Serial.println(input);
  Serial.print("Out: ");
  Serial.println(output);

  leftDriveMotor.drive(output, false);
  rightDriveMotor.drive(output, false);
}