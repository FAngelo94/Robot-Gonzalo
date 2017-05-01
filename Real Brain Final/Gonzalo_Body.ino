/*
 * Servo Gonzalo
 * This arduino manage the sonar and movements of body
 */
#include <Servo.h>
#include <NewPing.h>

#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

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
int distR[3], distL[3], distC[3];
double sonarTime;
//Servo for the rotation and its position
#define BODY_PIN 6
Servo bodyMotor;

//Variable to manage Hello
long helloTime;

int r,c,l;
static int MAX=3;

void setup() {
  //BODY
  Serial.begin(9600);
  bodyMotor.attach(BODY_PIN);
  bodyMotor.write(90);
  delay(500);
  bodyMotor.detach();
  for(int i=0;i<MAX;i++)
  {
    distR[i]=right.ping_median();
    distR[i]=right.convert_cm(distR[i]);
  }
  delay(30);
  for(int i=0;i<MAX;i++)
  {
    distC[i]=center.ping_median();
    distC[i]=center.convert_cm(distC[i]);
  }
  delay(30); 
  for(int i=0;i<MAX;i++)
  {
    distL[i]=left.ping_median(); 
    distL[i]=left.convert_cm(distL[i]);
  }
  //Set up hello
  helloTime=0;
  c=400;
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delay(1000);
}
void loop() {
  checkSonar();
  /*Serial.print("CCCCCCC=");
  Serial.println(c);*/
  if(millis()-helloTime>=4000 && (c<170 && c!=0))
  {
    Serial.println("CIAO");
    sayHello();
    helloTime=millis();
  }
}

//MANAGE SONAR AND ROTATION OF ROBOT
void checkSonar()
{
  for(int i=0;i<MAX;i++)
    distR[i]=distR[i+1];
  distR[MAX-1]=right.ping_median(5);
  distR[MAX-1]=right.convert_cm(distR[MAX-1]);
  //delay(30);
  for(int i=0;i<MAX;i++)
    distC[i]=distC[i+1];
  distC[MAX-1]=center.ping_median(5);
  distC[MAX-1]=center.convert_cm(distC[MAX-1]);
  //delay(30);
  for(int i=0;i<MAX;i++)
    distL[i]=distL[i+1]; 
  distL[MAX-1]=left.ping_median(5); 
  distL[MAX-1]=left.convert_cm(distL[MAX-1]);
  Serial.print("VR=");
  Serial.print(distR[MAX-1]);
  Serial.print(" - ");
  
  Serial.print("VC=");
  Serial.print(distC[MAX-1]);
  Serial.print(" - ");
  
  Serial.print("VL=");
  Serial.println(distL[MAX-1]);
  //delay(30);
  r=distR[MAX-1],c=distC[MAX-1],l=distL[MAX-1];
  if(distR[MAX-1]==0 || distC[MAX-1]==0 || distL[MAX-1]==0)
  {
    for(int i=0;i<MAX;i++)
    {
      if(distR[i]!=0)
        r=distR[i];
      if(distC[i]!=0)
        c=distC[i];
      if(distL[i]!=0)
        l=distL[i];
    }
  }
  Serial.print("R=");
  Serial.print(r);
  Serial.print(" - ");
  
  Serial.print("C=");
  Serial.print(c);
  Serial.print(" - ");
  
  Serial.print("L=");
  Serial.println(l);
  
  if(r==0)
    r=400;
  if(c==0)
    c=400;
  if(l==0)
    l=400;
  if(c<20)//if there is someone in front of him he doesn't do useless movement
    return; 
  
  bodyMotor.attach(BODY_PIN);
  if(r+20<c && r+20<l){
    int angolo=60;
    if(r<120)
      angolo=40;
    if(r<100)
      angolo=30;
    for(int i=0;i<angolo;i++)
    {
      if(bodyMotor.read()>30)
        bodyMotor.write(bodyMotor.read()-1);
      else 
        break;
      delay(20);
    }
    
  }
  if(l+20<c && l+20<r){
    int angolo=60;
    if(l<120)
      angolo=40;
    if(l<100)
      angolo=30;
    for(int i=0;i<angolo;i++)
    {
      if(bodyMotor.read()<130)
        bodyMotor.write(bodyMotor.read()+1);
      else
        break;
      delay(20);
    }
  }
  bodyMotor.detach();
  /*Serial.print("Ping1: ");
  Serial.print(distR ); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
  Serial.print("Ping2: ");
  Serial.print(distC ); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
  Serial.print("Ping3: ");
  Serial.println(distL ); // Send ping, get distance in cm and print result (0 = outside set distance range)*/
}

void sayHello()
{
  /*
   * Send to the secondary arduino the command to say hello
   */
  digitalWrite(2,HIGH);
  delay(300);
  digitalWrite(2,LOW);
}

