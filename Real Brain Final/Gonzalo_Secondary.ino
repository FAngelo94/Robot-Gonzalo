/*
 * Angelo Falci 
 * Emanuele Schiavon
 * Valentina Favaro
 * Julian Gallego
 * Soheil Ghanbari
 */
#include <Servo.h>
#include <DFRobotDFPlayerMini.h>7
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

//HAND WITH FOOD
#define FOOD_PIN 7
#define PIN_SENSOR_1 A0
#define PIN_SENSOR_2 A1
#define PIN_SENSOR_3 A2
Servo foodHand;
int positionFoodHand;
long foodTime;
long rechargeFoodTime;//when food hand moved then it doesn't work for a while
boolean foodMoveFast;
int oldValue[3];
int newValue[3];
int offerLed=4;

//HELLO HAND
#define HELLO_PIN 6
Servo helloHand;
int positionHelloHand;
long helloTime;
long helloInterval;
int helloDirection;

//eyes
#define GREEN 9
#define BLU 10
#define RED 11

//speak
SoftwareSerial mySoftwareSerial(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

//face
#define face1 5
#define face2 4
Servo faceS1, faceS2;
long mustacheTime;
long mustacheDirection;
long mustacheInterval;
int mustachePosition;

void setup() {
  //HAND WITH FOOD
  positionFoodHand=180;
  foodHand.attach(FOOD_PIN);
  foodHand.write(positionFoodHand); 
  foodMoveFast=false;
  foodTime=0;
  rechargeFoodTime=0;
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
  helloInterval=0;
  helloDirection=0;
  //eye setup
  pinMode(GREEN, OUTPUT);  
  pinMode(BLU, OUTPUT);  
  pinMode(RED, OUTPUT);
  blueEye();
  //Set up mustache
  faceS1.attach(face1);
  faceS2.attach(face2);
  //faceS1.write();
  //faceS2.write();
  mustacheTime=-3000;
  mustacheDirection=0;
  mustacheInterval=0;
  mustachePosition=90;
  sadMustache();
  delay(2000);
  helloHand.detach();
  foodHand.detach();
  helloTime=millis();
  //Body Arduino send in pin 2 the signal when a person is detected in front of it
  pinMode(2, INPUT);
  //Set up the DF Player
  mySoftwareSerial.begin(9600);
  //Serial.begin(115200);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    while(true);
  }
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  //Serial.begin(115200);
}

void loop() {
  checkDistance();
  //moveHandFast();
  sayhi();
  laughMustache();
  sadMustache();
}
//Check if the signal of "near person" is activated or not
void checkDistance(){
   
    if(digitalRead(2)==HIGH && millis()-helloTime>=3000)
    {
      //Serial.println("CIAO");
        helloTime=millis();
    }
}

//MOVE FAST THE HAND IF A PERSON TRY TO KEEP FOOD
//When someone try to keep food Gonzalo move hand, say "ahahah" and move mustache to laugh for about 2 second,
//and then his hand return in the original position
void moveHandFast()
{
  //Serial.println(foodMoveFast);
  if(foodMoveFast==false && millis()-foodTime>=1000 && millis()-rechargeFoodTime>0)
  {
    newValue[0]=analogRead(PIN_SENSOR_1);
    newValue[1]=analogRead(PIN_SENSOR_2);
    newValue[2]=analogRead(PIN_SENSOR_3);
   /* Serial.print("FOTO=");
    Serial.print(newValue[0]);
    Serial.print("-");
    Serial.print(newValue[1]);
    Serial.print("-");
    Serial.println(newValue[2]);*/
    if(-oldValue[0]+newValue[0]>5 || -oldValue[1]+newValue[1]>5 || -oldValue[2]+newValue[2]>5)
    {//A person is trying to take food
      foodMoveFast=true;
    }
    oldValue[0]=newValue[0];
    oldValue[1]=newValue[1];
    oldValue[2]=newValue[2];
    
    foodTime=millis();
  }
  if(foodMoveFast==true)
  {
      redEye();
      foodHand.attach(FOOD_PIN);
      for(int i=180;i>120;i=i-5)
      {
        foodHand.write(i);
        delay(2);
      }
    speak("fun");
    mustacheTime=millis();
      foodMoveFast=false;
      foodHand.detach();
      foodTime=millis();
      oldValue[0]=analogRead(PIN_SENSOR_1);
      oldValue[1]=analogRead(PIN_SENSOR_2);
      oldValue[2]=analogRead(PIN_SENSOR_3);
      helloTime=millis()+4000; //After move hand fast Gonzalo don't say hi for a while
      rechargeFoodTime=millis()+5000;//After move hand fast Gonzalo don't move the food hand again for a while
      blueEye();
  }
  if(foodMoveFast==false && foodHand.read()<=120 && millis()-helloTime>-1000 && millis()-helloTime<0)
  {
    foodHand.attach(FOOD_PIN);
      for(int i=120;i<180;i++)
      {
        foodHand.write(i);
        delay(10);
      }
      foodHand.detach();
    normalMustache();
  }
}

//Robot say hi for 3 seconds when the body arduino give the signal
void sayhi()
{
  if(millis()-helloTime<3000 && millis()-helloInterval>20)
  {
    //Serial.println("sto salutando");
  if(!helloHand.attached())  
  {
    helloHand.attach(HELLO_PIN);
    normalMustache();
    speak("hi");
    greenEye();
  }
  if(helloDirection==0)
    {
      positionHelloHand=positionHelloHand+2;
      if(positionHelloHand>=110)
      {
        helloDirection=1;
      }
    }
    else
    {
      positionHelloHand=positionHelloHand-2;
      if(positionHelloHand<=70)
      {
        helloDirection=0;
      }
    } 
  helloHand.write(positionHelloHand);
  helloInterval=millis();
  }
  if(millis()-helloTime>=3000 && helloHand.attached())
  {//detach the servo when Gonzalo not say hello
    helloHand.detach();
    blueEye();
  }
}

void blueEye()
{
  digitalWrite(GREEN, LOW);  
  digitalWrite(BLU, LOW);  
  digitalWrite(RED, HIGH);
}
void redEye()
{
  digitalWrite(GREEN, HIGH);  
  digitalWrite(BLU, LOW);  
  digitalWrite(RED, LOW);
}
void greenEye()
{
  digitalWrite(GREEN, LOW);  
  digitalWrite(BLU, HIGH);  
  digitalWrite(RED, LOW);
}

//MANAGE ROBOT SPEAKING
void speak(String s)
{
  if(s=="hi")
  {
    myDFPlayer.play(2);
  }
  if(s=="fun")
  {
    myDFPlayer.play(1);  
  }
}

//MANAGE EYEBROW AND MUSTACHE
void normalMustache()
{

  faceS1.attach(face1);
  faceS2.attach(face2);

  //faceS1.write();
  //faceS2.write();
  delay(50);
  faceS1.detach();
  faceS2.detach(); 
}
//default position when there isn't anyone
//Gonzalo become sad when it doesn't interact with anyone for a while
void sadMustache()
{
  if(millis()-helloTime>10000 && millis()-foodTime>1000)
  {
    faceS1.attach(face1);
    faceS2.attach(face2);

    //faceS1.write();
    //faceS2.write();
    delay(50);
    faceS1.detach();
    faceS2.detach(); 
  }
}
void laughMustache()
{
  if(millis()-mustacheTime<2000 && millis()-mustacheInterval>15)
  {
    if(!faceS1.attached())
      faceS1.attach(face1);
    if(!faceS2.attached())
      faceS2.attach(face2);
    if(mustacheDirection==0)
    {
      mustachePosition=mustachePosition+2;
      if(mustachePosition>100)
        mustacheDirection=1;
    }
    else
    {
      mustachePosition=mustachePosition-2;
      if(mustachePosition<80)
        mustacheDirection=0;
    }
    //faceS1.write();
    //faceS2.write();
    mustacheInterval=millis();
  }
  if(millis()-mustacheTime>2000)
  {
    if(faceS1.attached())
      faceS1.detach();
    if(faceS2.attached())
      faceS2.detach();
  }
}
