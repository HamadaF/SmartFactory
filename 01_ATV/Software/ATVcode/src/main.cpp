#include <Arduino.h>
#include <QTRSensors.h> //Bibliothek des QTR-8RC LINE SENSOR
//#include <iostream>

//Function declarations
void autoCalibration();
void set_motors(int leftMotorSpeed, int rightMotorspeed);
void manualCalibration(const int *manual_calibration_values);
void showDebug();

// ----------------------- Pin Definitions ---------------------------------- //
#define speedPinA 3       //pwm pin left motors
#define speedPinB 8      // pwm pin right motors
#define IN1 4             // digital pins for motor direction
#define IN2 5
#define IN3 6
#define IN4 7

//QRT-RC8 initialisierung
#define KP 2                 //experiment to determine this, start by something small that just makes your bot follow the line at a slow speed
#define KD 5                 //experiment to determine this, slowly increase the speeds and adjust this value. ( Note: Kp < Kd)
#define M1_minumum_speed 0   //minimum speed of the Motor1
#define M2_minumum_speed 0   //minimum speed of the Motor2
#define M1_maximum_speed 255 //max. speed of the Motor1
#define M2_maximum_speed 255 //max. speed of the Motor2
//#define MIDDLE_SENSOR 4      //number of middle sensor used
#define NUM_SENSORS 8        //number of sensors used
#define TIMEOUT 2500         //waits for 2500 us for sensor outputs to go low
//#define EMITTER_PIN 2        //emitterPin is the Arduino digital pin that controls whether the IR LEDs are on or off. Emitter is controlled by digital pin 2
#define DEBUG 0

const int manual_calibration[] = {708, 864, 812, 836, 784};

//sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsRC qtrrc((unsigned char[]){A7,A6,A5,A4,A3,A2,A1,A0}, NUM_SENSORS, TIMEOUT);

int lastError = 0;
int last_proportional = 0;
int integral = 0;

void setup()
{
  Serial.begin(115200);
  // showDebug();
  // delay(2000);
  autoCalibration();
  // showDebug();
  // delay(2000);
  // Serial.println("AutoCalibration Done.");
  // Serial.println("Loading values from manual calibration before ...");
  // manualCalibration(manual_calibration);
  // showDebug();


}

void loop()
{
  unsigned int sensors[8];
  int position = qtrrc.readLine(sensors); // wert von 0 bis 1000*NUM_SENSORS
  int error = position - (1000*(NUM_SENSORS-1))/2;            // Anpassung des Wertes, damit die Mitte bei 0 ist
  Serial.println(position);
  int motorSpeed = KP * error + KD * (error - lastError); // Setzen des neuen motorSpeed, evt. müssen die KP- und KD-Regler angepasst werden
  lastError = error;

  int leftMotorSpeed = M1_minumum_speed + motorSpeed; // Der minimale motorSpeed wird um einen neuen erhöht. In userem Fall ist der minimumSpeed 0. D.h. liest der Sensor eine Null, so bleibt das Fahrzeug stehen. Siehe hierzu das Protokoll vom 27.01.18
  int rightMotorSpeed = M2_minumum_speed - motorSpeed;
  set_motors(leftMotorSpeed, rightMotorSpeed);
}

void set_motors(int leftMotorSpeed, int rightMotorspeed)
{
  // Anpassung der Geschwindigkeiten, falls sie einen Maximal- bzw. Minimalwert über(unter)schreiten
  if (leftMotorSpeed > M1_maximum_speed)
    leftMotorSpeed = M1_maximum_speed;
  if (rightMotorspeed > M2_maximum_speed)
    rightMotorspeed = M2_maximum_speed;
  if (leftMotorSpeed < 0)
    leftMotorSpeed = 0;
  if (rightMotorspeed < 0)
    rightMotorspeed = 0;

  analogWrite(speedPinA, leftMotorSpeed);
  analogWrite(speedPinB, rightMotorspeed);
  digitalWrite(IN1, HIGH); // in dieser Position fährt das FTS vorwärts; für RW alle HIGHs und LOWs umkehren;
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void manualCalibration(const int *fixed_values) {
  for (int i=0; i < NUM_SENSORS; i++) {
    qtrrc.calibratedMinimumOn[i] = fixed_values[i];
    qtrrc.calibratedMaximumOn[i] = 2500;
  }
}

void autoCalibration()
{

  int i;
  for (i = 0; i < 250; i++)
  {
    qtrrc.calibrate(QTR_EMITTERS_ON);

    delay(20);
  }

}

void showDebug() {
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      Serial.print(qtrrc.calibratedMinimumOn[i]);
      Serial.print(' ');
    }
    Serial.println();

    for (int i = 0; i < NUM_SENSORS; i++)
    {
      Serial.print(qtrrc.calibratedMaximumOn[i]);
      Serial.print(' ');
    }
    Serial.println();
    Serial.println();
}
