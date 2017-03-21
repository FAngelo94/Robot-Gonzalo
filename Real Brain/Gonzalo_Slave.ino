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
//time wait before send
long timeSend;

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
  Wire.begin();
  timeSend=0;
  delay(1000);
}

void loop() {
  checkSonar();
  if(millis()-timeSend>=500)
  {
    sendDistanceToServer();
    timeSend=millis();
  }
  /*for(int i=0;i<100;i++)
    {
        bodyMotor.write(i);
      delay(20);
    }
    for(int i=100;i>0;i--)
    {
        bodyMotor.write(i);
      delay(20);
    }
  delay(5000);*/
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

void sendDistanceToServer()
{
  /*
   * Server always wait for 3 bytes, so if distC<100 I add some
   * letters in order to have 3 bytes
   */
  Wire.beginTransmission(8); // transmit to device #8
  int d=distC;
  char c;
  if(distC<10)
  {
    c=distC+48;
    Wire.write('0');
    Wire.write('0');
    Wire.write(c);
  }
  if(distC<100 && distC>=10)
  {
    Wire.write('0');
    int tmp=d/10;
    c=tmp+48;
    Wire.write(c);
    tmp=d-tmp*10;
    c=tmp+48;
    Wire.write(c);
    
  }
  if(distC>100)
  {
    int tmp=d/100;
    c=tmp+48;
    Wire.write(c);
    tmp=(d-tmp*100)/10;
    c=tmp+48;
    Wire.write(c);
    tmp=(d-tmp*10-d/100*100);
    c=tmp+48;
    Wire.write(c);
  }
  Wire.endTransmission();    // stop transmitting 
}

