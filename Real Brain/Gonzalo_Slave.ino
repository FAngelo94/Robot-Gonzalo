/*
 * Servo Gonzalo
 * This arduino manage the sonar and movements of body
 */
#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define TRIGGER_PIN1  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN1     11  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN2  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN2    9  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN3  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN3    7  // Arduino pin tied to echo pin on the ultrasonic sensor.

//Right
NewPing right(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//Center
NewPing center(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//Left
NewPing left(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//distance of every sonar
int distR, distL, distC;
double sonarTime;
//Servo for the rotation and its position
#define BODY_PIN 6
Servo bodyMotor;

void setup() {
  //BODY
  Serial.begin(9600);
  bodyMotor.attach(BODY_PIN);
  bodyMotor.write(90);
  delay(500);
  bodyMotor.detach();
  distR=right.ping_cm(); 
  delay(30);
  distC=center.ping_cm();
  delay(30); 
  distL=left.ping_cm(); 
  if(distL==0)
    distL=400;
  if(distC==0)
    distC=400;
  if(distR==0)
    distR=400;
  //set up the communication 
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  delay(1000);
}

void loop() {
  checkSonar();
}

//MANAGE SONAR AND ROTATION OF ROBOT
void checkSonar()
{
  distR=right.ping_cm();
  delay(30);
  distC=center.ping_cm();
  delay(30);
  distL=left.ping_cm();
  delay(30);
  if(distL==0)
    distL=400;
  if(distC==0)
    distC=400;
  if(distR==0)
    distR=400; 
  bodyMotor.attach(BODY_PIN);
  if(distR+10<distC && distR+10<distL){
    for(int i=0;i<60;i++)
    {
      if(bodyMotor.read()>30)
        bodyMotor.write(bodyMotor.read()-1);
      else 
        break;
      delay(20);
    }
  }
  if(distL+10<distC && distL+10<distR){
    for(int i=0;i<60;i++)
    {
      if(bodyMotor.read()<130)
        bodyMotor.write(bodyMotor.read()+1);
      else
        break;
      delay(20);
    }
  }
  bodyMotor.detach();
  Serial.print("Ping1: ");
  Serial.print(distR ); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
  Serial.print("Ping2: ");
  Serial.print(distC ); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
  Serial.print("Ping3: ");
  Serial.println(distL ); // Send ping, get distance in cm and print result (0 = outside set distance range)
    
  Serial.print("ANGOLO=");
  Serial.println(bodyMotor.read());
}

void requestEvent() {
  sendDistanceToServer();
}

void sendDistanceToServer()
{
  /*
   * Server always wait for 3 bytes, so if distC<100 I add some
   * letters in order to have 3 bytes
   */
  if(distC<10)
    Wire.write("xx");
  if(distC<100 && distC>=10)
    Wire.write("x");
  Wire.write(distC);
}

