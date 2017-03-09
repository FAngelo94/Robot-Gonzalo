/*
 * Angelo Falci 
 * Emanuele Schiavon
 * Valentina Favaro
 * Julian Gallego
 * Soheil Ghanbari
 */
#include <Servo.h>
#include <NewPing.h>

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
int positionBody;//save the position that body must be reach
int realBody;//save the instant position that body has
long bodyTime;
//Variable to check if a person is near for more than 2/3 seconds
int seePerson;
int lastDistSeePerson;
int checkPerson;
//Time offerFood
int timeOffer;//count for how many time the robot is waiting the person

//HAND WITH FOOD
#define FOOD_PIN 5
#define PIN_SENSOR_1 A0
#define PIN_SENSOR_2 A1
#define PIN_SENSOR_3 A2
Servo foodHand;
int positionFoodHand;
long foodTime;
boolean foodMoveFast;
int oldValue[3];
int newValue[3];
int offerLed=4;

//HELLO HAND
#define HELLO_PIN 4
Servo helloHand;
int positionHelloHand;
long helloTime;
long hiTime;
int helloDirection;

//eyes
int eye=3;

void setup() {
  //BODY
  Serial.begin(115200);
  positionBody=90;
  realBody=90;
  bodyMotor.attach(BODY_PIN);
  bodyMotor.write(positionBody);
  delay(500);
  sonarTime=0;
  bodyTime=0;
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
  //PERSON NEAR
  seePerson=0;
  lastDistSeePerson=0;
  checkPerson=0;
  //HAND WITH FOOD
  positionFoodHand=130;
  foodHand.attach(FOOD_PIN);
  foodHand.write(positionFoodHand); 
  foodMoveFast=false;
  foodTime=0;
  oldValue[0]=analogRead(PIN_SENSOR_1);
  oldValue[1]=analogRead(PIN_SENSOR_2);
  oldValue[2]=analogRead(PIN_SENSOR_3);
  pinMode(offerLed, OUTPUT);
  digitalWrite(offerLed, LOW);
  //HELLO HAND
  positionHelloHand=90;
  helloHand.attach(HELLO_PIN);
  helloHand.write(positionHelloHand);
  helloTime=0;
  hiTime=0;
  helloDirection=0;
  pinMode(eye, OUTPUT);
  digitalWrite(eye, LOW);  
  int sensorHigh;
  int sensorLow;
  /*while(millis()<5000)
  {
    sensorValue=analogRead(A0);
    if(sensorValue>sensorHigh)
    {
      sensorHigh=sensorValue;
    }
    if(sensorValue<sensorLow)
    {
      sensorLow=sensorValue;
    }
  }*/
    delay(5000);
  digitalWrite(eye, HIGH);
}

void loop() {
  checkSonar();
  //nearPerson();
  //happy();
  //moveHandFast();
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
void readSonar()
{
  long delayRead=sonarTime-millis();
  distR=right.ping_cm();
  //delay(30);
  //distR=right.convert_cm(distR);
  distC=center.ping_cm();
  //delay(30);
  //distC=center.convert_cm(distC);
  distL=left.ping_cm();
  //distL=left.convert_cm(distL);
  /*if(delayRead>=0 && delayRead<=30)
  {
    //distR=right.ping_cm(); 
    //delay(30);
  }
  //if(delayRead>=60 && delayRead<=90)
  {
    /distC=center.ping_cm(); 
    //delay(30);
  }
  //if(delayRead>=120 && delayRead<=150)
  {
    distL=left.ping_cm(); 
    //delay(30);
  }*/
  if(distL==0)
    distL=400;
  if(distC==0)
    distC=400;
  if(distR==0)
    distR=400; 
}
void detachBody()
{
  if(realBody==positionBody && bodyMotor.attached())
    bodyMotor.detach();
}
void attachBody()
{
  if(realBody!=positionBody && millis()-bodyTime>=10)
  {
    if(realBody<positionBody)
      realBody=realBody+1;
    else
      realBody=realBody-1;
    if(!bodyMotor.attached())
      bodyMotor.attach(BODY_PIN);
    bodyMotor.write(realBody);
    bodyTime=millis(); 
  }
}
void moveRight()
{
   if(positionBody>30){
        positionBody=positionBody-5;
   }
   else{
    positionBody=30;}
}

void moveLeft()
{
   if(positionBody<130){
        positionBody=positionBody+5;
    }
   else{
    positionBody=130;}
   
}

//WHAT DO WHEN A PERSON IS NEAR
void nearPerson()
{
  if(distC<10 && checkPerson==0)//person is near
  {
    seePerson=millis();
    lastDistSeePerson=distC;
    checkPerson=1;
  }
  if(seePerson-millis()>=2000 && checkPerson==1)
  {
    if(distC<=lastDistSeePerson)//person is also near after 2 seconds
    {
      checkPerson=2;
      seePerson=millis();//Turn on led to offer the food
    }
  }
  if(seePerson-millis()>=4000 && checkPerson==1)
  {//Person go away, return to check
    checkPerson=0;
  }
  if(checkPerson==2)
  {
    offerFood();
  }
}
void offerFood()
{
  if(millis()-seePerson>=3000)
  {//offer food
  digitalWrite(offerLed, HIGH);
  }
  else
  {//finish to offer food
  digitalWrite(offerLed, LOW);
  checkPerson=0;//return to check
  }
}
//MOVE FAST THE HAND IF A PERSON TRY TO KEEP FOOD
void moveHandFast()
{
  //Serial.println(oldValue[0]);
  if(foodMoveFast==false)
  {
  newValue[0]=analogRead(PIN_SENSOR_1);
  newValue[1]=analogRead(PIN_SENSOR_2);
  newValue[2]=analogRead(PIN_SENSOR_3);
  if(oldValue[0]-newValue[0]>1)
  {//A person is trying to take food
    foodMoveFast=true;
    foodTime=millis();
  }
  oldValue[0]=newValue[0];
  oldValue[1]=newValue[1];
  oldValue[2]=newValue[2];
  }
  else
  {
    if(foodTime==0 || millis()-foodTime>=15)
    {
      positionFoodHand=positionFoodHand-5;    
      foodHand.write(positionFoodHand);
      foodTime=millis();
      if(positionFoodHand<80)
      {
        foodMoveFast=false;
        foodTime=millis()+2000;//I put the hand in "offered position" after 2 seconds
      }
    }
  }
  if(foodMoveFast==false && positionFoodHand<130 && (foodTime==0 || millis()-foodTime>=20))
  {
  positionFoodHand=positionFoodHand+5;
  foodTime=millis();
  foodHand.write(positionFoodHand);
  }
}

//ROBOT IS SAD (SWING SLOW) OR HAPPY (SAY HELLO WITH ARM)
void happy()
{
  //The hand move in order to say "hello" if the person is not too far to take its attention and
  //stop when the person come near beacuse we have its attention 
  if((helloTime==0 || millis()-helloTime>=20) && (distC<=100) && (distC>=50) && millis()-hiTime>=3000)
  {
    digitalWrite(eye, HIGH);
    helloTime=millis();
    hiTime=millis();
  }
  if(millis()-hiTime>=3000)
  {
    digitalWrite(eye, LOW);
  }
  /*
   * When Gonzalo see a person say hi for 3 seconds, then stop and if there is someone restart
   */
  sayhi();
  
}
void sayhi()
{
  if((helloTime==0 || millis()-helloTime>=20) && millis()-hiTime<3000)
  {
    //Serial.print("distanza ciao");
    //Serial.println(distC);
    if(helloDirection==0)
    {
      positionHelloHand=positionHelloHand+10;
      if(positionHelloHand>=125)
      {
        helloDirection=1;
      }
    }
    else
    {
      positionHelloHand=positionHelloHand-10;
      if(positionHelloHand<=55)
      {
        helloDirection=0;
      }
    } 
    helloTime=millis();
    helloHand.write(positionHelloHand);
  }
}
void sad()
{
  
}

//MANAGE ROBOT SPEAKING
void speak(int i)
{
  
}

