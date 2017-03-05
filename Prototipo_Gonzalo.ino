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
int positionBody;
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
int helloDirection;

//eyes
int eye=3;

void setup() {
  //BODY
  Serial.begin(115200);
  positionBody=90;
  bodyMotor.attach(BODY_PIN);
  bodyMotor.write(positionBody);
  delay(500);
  //bodyMotor.detach();
  sonarTime=0;
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
  detachServo();
  //nearPerson();
  happy();
  moveHandFast();
}

//MANAGE SONAR AND ROTATION OF ROBOT
void checkSonar()
{
  if(millis()-sonarTime>=200)
  {
    //Maybe I can put the read in a method without delay
    delay(30); 
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
  
      Serial.print("Ping1: ");
    Serial.print(distR ); // Send ping, get distance in cm and print result (0 = outside set distance range)
    
    Serial.print("Ping2: ");
    Serial.print(distC ); // Send ping, get distance in cm and print result (0 = outside set distance range)
    
    Serial.print("Ping3: ");
    Serial.println(distL ); // Send ping, get distance in cm and print result (0 = outside set distance range)
    
     
    boolean check=false;
    if(distR<distC+10 && distR<distL+10){
      //bodyMotor.attach(BODY_PIN);  
      moveRight();
      check=true;
      }
    if(distL<distC+10 && distL<distR+10){
      //bodyMotor.attach(BODY_PIN);  
      moveLeft(); 
      check=true;
      }  
     if(bodyMotor.attached() && check==true)
     {
        bodyMotor.write(positionBody);
        Serial.println("MI MUOVO");
     }
     sonarTime=millis();
  }
}
void detachServo()
{
  //if(bodyMotor.read()==positionBody && millis()-sonarTime>=100)
    //bodyMotor.detach();
}
void moveRight()
{
   if(positionBody>30){
        positionBody=positionBody-10;
        bodyMotor.write(positionBody);
   }
   else{
    //bodyMotor.detach();
    positionBody=30;}
}

void moveLeft()
{
   if(positionBody<130){
        positionBody=positionBody+10;
        bodyMotor.write(positionBody);
    }
   else{
   //bodyMotor.detach();
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
  Serial.println(oldValue[0]);
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
  if((helloTime==0 || millis()-helloTime>=15) && (distC<=100 || distR<=100 || distL<=100))
  {
    if(helloDirection==0)
    {
      positionHelloHand=positionHelloHand+5;
      if(positionHelloHand>=125)
      {
        helloDirection=1;
      }
    }
    else
    {
      positionHelloHand=positionHelloHand-5;
      if(positionHelloHand<=55)
      {
        helloDirection=0;
      }
    } 
    digitalWrite(eye, HIGH);
    helloTime=millis();
    helloHand.write(positionHelloHand);
  }
  if(distC>100 && distR>100 && distL>100)
    digitalWrite(eye, LOW);
}
void sad()
{
  
}

//MANAGE ROBOT SPEAKING
void speak(int i)
{
  
}

