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
boolean resetFoodHand;
//boolean foodMoveFastActive;
int oldValue[10];
int newValue;
boolean activeFoodHand;

//HELLO HAND
#define HELLO_PIN 6
Servo helloHand;
int positionHelloHand;
long helloTime;
long helloInterval;
int helloDirection;
long rechargeHi;
boolean enter;

//eyes
#define GREEN 9
#define BLU 10
#define RED 11

//speak
SoftwareSerial mySoftwareSerial(12, 13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int enterCount;

//face
#define face1 5
#define face2 4
Servo eyeBrown, mustache;
long mustacheTime;
long mustacheDirection;
long mustacheInterval;
int mustachePosition;

//Pause
boolean sleep;

void setup() {
  //HAND WITH FOOD
  positionFoodHand=170;
  foodHand.attach(FOOD_PIN);
  foodHand.write(positionFoodHand); 
  foodMoveFast=false;
  resetFoodHand=false;
  foodTime=0;
  rechargeFoodTime=0;
  activeFoodHand=false;
  for(int i=0;i<10;i++)
  {
    oldValue[i]=analogRead(PIN_SENSOR_1);
    delay(50);
  }
  //HELLO HAND
  positionHelloHand=90;
  helloHand.attach(HELLO_PIN);
  helloHand.write(positionHelloHand);
  helloTime=0;
  helloInterval=0;
  helloDirection=0;
  rechargeHi=0;
  enter=false;
  //eye setup
  pinMode(GREEN, OUTPUT);  
  pinMode(BLU, OUTPUT);  
  pinMode(RED, OUTPUT);
  blueEye();
  //Set up mustache
  eyeBrown.attach(face1);
  mustache.attach(face2);
  eyeBrown.write(90);
  mustache.write(90);
  mustacheTime=-3000;
  mustacheDirection=0;
  mustacheInterval=0;
  mustachePosition=90;
  sadMustache();
  delay(2000);
  helloHand.detach();
  eyeBrown.detach();
  mustache.detach();
  foodHand.detach();
  helloTime=millis();
  //Body Arduino send in pin 2 the signal when a person is detected in front of it
  pinMode(2, INPUT);
  //Set up the DF Player
  mySoftwareSerial.begin(9600);
  enterCount=0;
  //Serial.begin(115200);
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    while(true);
  }
  myDFPlayer.volume(10);  //Set volume value. From 0 to 304
  sleep=false;
}

void loop() {
  checkDistance();
  moveHandFast();
  sayhi();
  laughMustache();
  sadMustache();
}
//Check if the signal of "near person" is activated or not
void checkDistance(){
   
    if(digitalRead(2)==HIGH)
    {
      //Serial.println("CIAO");
        helloTime=millis();
        activeFoodHand=true;
    }
}

//MOVE FAST THE HAND IF A PERSON TRY TO KEEP FOOD
//When someone try to keep food Gonzalo move hand, say "ahahah" and move mustache to laugh for about 2 second,
//and then his hand return in the original position
void moveHandFast()
{
  
  //Serial.println(foodMoveFast);
  long diff=millis()-rechargeFoodTime;
  if(foodMoveFast==false && millis()-foodTime>=200 && diff>0)
  {
    double maxL=0;
    for(int i=0;i<10;i++)
      if(oldValue[i]>maxL)
        maxL=oldValue[i];
    double diff=((double)newValue-(double)maxL)/((double)maxL)*100;
    newValue=analogRead(PIN_SENSOR_1);
    
    /*Serial.print("FOTO=");
    Serial.print(newValue);
    Serial.print(" - ");
    Serial.println(maxL);
    Serial.print("DIF=");
    Serial.println(diff);*/
    
    if(diff<-15 && activeFoodHand==true)
    {//A person is trying to take food
      foodMoveFast=true;
      activeFoodHand=false;
      redEye();
    }
    else
    {
      for(int i=0;i<9;i++)
        oldValue[i]=oldValue[i+1];
      oldValue[9]=newValue;
    }
    foodTime=millis();
  }
  if(foodMoveFast==true)
  {
      
      if(helloHand.attached())
        helloHand.detach();
       
      rechargeHi=millis()+5000; //After move hand fast Gonzalo don't say hi for a while
      foodHand.attach(FOOD_PIN);
      for(int i=170;i>120;i=i-3)
      {
        foodHand.write(i);
        delay(15);
      }
      foodHand.detach();
      speak("fun");
      mustacheTime=millis();
      foodMoveFast=false;
      resetFoodHand=true;
      foodTime=millis();
      rechargeFoodTime=millis()+10000;//After move hand fast Gonzalo don't move the food hand again for a while
      
  }
  if(resetFoodHand==true && rechargeFoodTime-millis()<5000)
  {
    for(int i=0;i<10;i++)
    {
      oldValue[i]=analogRead(PIN_SENSOR_1);
      delay(20);
    }
    foodHand.attach(FOOD_PIN);
      for(int i=120;i<170;i++)
      {
        foodHand.write(i);
        delay(20);
      }
      foodHand.detach();
      resetFoodHand=false;
    normalMustache();
    blueEye();
  }
}

//Robot say hi for 3 seconds when the body arduino give the signal
void sayhi()
{ 
  long diff=millis()-rechargeHi;
  if(millis()-helloTime<3000 && millis()-helloInterval>10 && helloTime!=0 && diff>0 && enter==false)
  {
    //Serial.println("sto salutando");
  if(!helloHand.attached() || sleep==true)  
  {
    if(!helloHand.attached()) 
      helloHand.attach(HELLO_PIN);
    happyMustache();
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
  if(millis()-helloTime>=3000 && helloHand.attached() && sleep==false)
  {//detach the servo when Gonzalo not say hello
    
    if(helloHand.attached())
      helloHand.detach();
    rechargeHi=millis()+8000;
    enter=true;
  }
  if(millis()-helloTime>=5000 && sleep==false)
  {//detach the servo when Gonzalo not say hello
    blueEye();
  }
  if(millis()-helloTime<3000 && millis()-helloInterval>20 && helloTime!=0 && diff>0 && enter==true)
  {
    if(helloHand.attached())
      helloHand.detach();
      normalMustache();
    speak("enter");
    helloTime=millis()-3000;
    rechargeHi=millis()+5000;
    enter=false;
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
  if(s=="enter")
  {
    if(enterCount==0)
      myDFPlayer.play(2);
    if(enterCount==1)
      myDFPlayer.play(5);
    if(enterCount==2)
      myDFPlayer.play(6);
    enterCount++;
    if(enterCount>2)
    enterCount=0;
  }
  if(s=="song")
  {
    myDFPlayer.play(1);  
  }
  if(s=="hi")
  {
    myDFPlayer.play(3); 
  }
  if(s=="fun")
  {
    myDFPlayer.play(4); 
  }
}

//MANAGE EYEBROW AND MUSTACHE
void normalMustache()
{
  if(!eyeBrown.attached())
    eyeBrown.attach(face1);
  if(!mustache.attached())
    mustache.attach(face2);

  eyeBrown.write(90);
  mustache.write(90);
  delay(200);
  
  eyeBrown.detach();
  mustache.detach(); 
}

void happyMustache()
{
  if(!eyeBrown.attached())
    eyeBrown.attach(face1);
  if(!mustache.attached())
    mustache.attach(face2);

  eyeBrown.write(20);
  mustache.write(150);
  delay(200);
  
  eyeBrown.detach();
  mustache.detach(); 
}
//default position when there isn't anyone
//Gonzalo become sad when it doesn't interact with anyone for a while
void sadMustache()
{
  long dist1=millis()-helloTime;
  long dist2=millis()-rechargeFoodTime;
  if(!(dist1>10000 && dist2>0))
    sleep=false;
  if(dist1>10000 && dist2>0)
  {
    if(!mustache.attached())
      mustache.attach(face2);
    if(mustacheDirection==0)
    {
      mustachePosition=mustachePosition+1;
      if(mustachePosition>120)
        mustacheDirection=1;
    }
    else
    {
      mustachePosition=mustachePosition-1;
      if(mustachePosition<60)
        mustacheDirection=0;
    }
    mustache.write(mustachePosition);
    if(!helloHand.attached())  
    {
      helloHand.attach(HELLO_PIN);
      normalMustache();
      speak("song");
      sleep=true;
    }
    if(helloDirection==0)
    {
      positionHelloHand=positionHelloHand+1;
      if(positionHelloHand>=110)
      {
        helloDirection=1;
      }
    }
    else
    {
      positionHelloHand=positionHelloHand-1;
      if(positionHelloHand<=70)
      {
        helloDirection=0;
      }
    } 
    if(!helloHand.attached())  
      helloHand.attach(HELLO_PIN);
    helloHand.write(positionHelloHand);
    delay(10);
  }
}
void laughMustache()
{
  if(millis()-mustacheTime<5000 && millis()-mustacheInterval>10)
  {
    if(!eyeBrown.attached())
    {
      eyeBrown.attach(face1);
      delay(10);
      eyeBrown.write(160);
    }
    if(!mustache.attached())
      mustache.attach(face2);
    if(mustacheDirection==0)
    {
      mustachePosition=mustachePosition+4;
      if(mustachePosition>120)
        mustacheDirection=1;
    }
    else
    {
      mustachePosition=mustachePosition-4;
      if(mustachePosition<60)
        mustacheDirection=0;
    }
    mustache.write(mustachePosition);
    mustacheInterval=millis();
  }
  if(millis()-mustacheTime>500)
  {
    if(eyeBrown.attached())
      eyeBrown.detach();
  }
  if(millis()-mustacheTime>5000)
  {
    if(mustache.attached())
      mustache.detach();
  }
}
