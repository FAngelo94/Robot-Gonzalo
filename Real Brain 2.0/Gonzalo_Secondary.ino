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

//Variable to check if a person is near for more than 2/3 seconds
int seePerson;
int lastDistSeePerson;
int checkPerson;
//Time offerFood
long timeOffer;//count for how many time the robot is waiting the person

//HAND WITH FOOD
#define FOOD_PIN 7
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
long speakTime;

//face
#define face1 13
#define face2 12
Servo faceS1, faceS2;

//Music
//SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
//DFRobotDFPlayerMini myDFPlayer;


void setup() {
  //HAND WITH FOOD
  positionFoodHand=180;
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
  helloInterval=0;
  helloDirection=0;
  //eye setup
  pinMode(GREEN, OUTPUT);  
  pinMode(BLU, OUTPUT);  
  pinMode(RED, OUTPUT);
  //communication with slave set up
  //setup eyes
  blueEye();
  //Set up the DF Player
  //mySoftwareSerial.begin(9600);
  //myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  delay(2000);
  helloHand.detach();
  foodHand.detach();
  helloTime=millis();
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  speakTime=0;
  //Serial.begin(9600);
}

void loop() {
  sayhi();
  requestEvent();
  moveHandFast();
  if(millis()-speakTime>=500)
  {
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }
}
//request to the slave to give the distance of central sonar
void requestEvent(){
    //Serial.print("Caratteri=");
    //Serial.print(digitalRead(2));
    if(digitalRead(2)==HIGH && millis()-helloTime>=3000)
    {
      //Serial.print(c);
        helloTime=millis();
        //Serial.println("ciao");
    }
    //Serial.println("Hola chica");
    //Serial.println();
}

//MOVE FAST THE HAND IF A PERSON TRY TO KEEP FOOD
void moveHandFast()
{
  //Serial.println(foodMoveFast);
  if(foodMoveFast==false && millis()-foodTime>=1000)
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
      for(int i=180;i>85;i=i-5)
      {
        foodHand.write(i);
        delay(2);
      }
      delay(1000);
      speak("fun");
      for(int i=85;i<180;i++)
      {
        foodHand.write(i);
        delay(10);
      }
      foodMoveFast=false;
      foodHand.detach();
      foodTime=millis();
      
      oldValue[0]=analogRead(PIN_SENSOR_1);
      oldValue[1]=analogRead(PIN_SENSOR_2);
      oldValue[2]=analogRead(PIN_SENSOR_3);
      helloTime=millis()+3000;
      blueEye();
  }
 /* if(foodMoveFast==false && positionFoodHand<130 && (foodTime==0 || millis()-foodTime>=20))
  {
    positionFoodHand=positionFoodHand+5;
    foodTime=millis();
    foodHand.write(positionFoodHand);
  }*/
}

//Robot say hi for 3 seconds when the body arduino said
void sayhi()
{
  if(millis()-helloTime<3000 && millis()-helloInterval>20)
  {
    /*Serial.print("Tempi=");
  Serial.print(helloTime);
  Serial.print("-");
  Serial.println(helloInterval);*/
  if(!helloHand.attached())  
  {
    helloHand.attach(HELLO_PIN);
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
    //myDFPlayer.play(1);
  }
  if(s=="fun")
  {
    //myDFPlayer.play(2);  
  }
  speakTime=millis();
}

//MANAGE EYEBROW AND MUSTACHE
void happyMustache()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach();
}
void normalMustache()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach(); 
}
void sadMustache()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach();
}
void happyEyeBrow()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach();
}
void normalEyeBrow()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach();
}
void sadEyeBrow()
{
  faceS1.attach(face1);
  faceS2.attach(face2);

  
  
  faceS1.detach();
  faceS2.detach();
}

