/*
 * Angelo Falci 
 * Emanuele Schiavon
 * Valentina Favaro
 * Julian Gallego
 * Soheil Ghanbari
 */
#include <Servo.h>
#include <Wire.h>

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

void setup() {
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
  helloInterval=0;
  helloDirection=0;
  //eye setup
  pinMode(GREEN, OUTPUT);  
  pinMode(BLU, OUTPUT);  
  pinMode(RED, OUTPUT);
  //communication with slave set up
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(requestEvent); // register event
  //setup eyes
  blueEye();
  delay(2000);
  helloHand.detach();
  Serial.begin(9600);
}

void loop() {
  sayhi();
  //moveHandFast();
}
//request to the slave to give the distance of central sonar
void requestEvent(int howMany){
    Serial.print("Caratteri=");
    if(Wire.available())
    {
      char c=Wire.read();
      Serial.print(c);
      if(c=='h')
        helloTime=millis();
    }
    Serial.println();
}

//MOVE FAST THE HAND IF A PERSON TRY TO KEEP FOOD
void moveHandFast()
{
  if(foodMoveFast==false && millis()-foodTime>100)
  {
    newValue[0]=analogRead(PIN_SENSOR_1);
    newValue[1]=analogRead(PIN_SENSOR_2);
    newValue[2]=analogRead(PIN_SENSOR_3);
    if(oldValue[0]-newValue[0]>1 || oldValue[1]-newValue[1]>1 || oldValue[2]-newValue[2]>1)
    {//A person is trying to take food
      foodMoveFast=true;
    }
    oldValue[0]=newValue[0];
    oldValue[1]=newValue[1];
    oldValue[2]=newValue[2];
    foodTime=millis();
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

//Robot say hi for 3 seconds when the body arduino said
void sayhi()
{
  if(millis()-helloTime<3000 && millis()-helloInterval>20)
  {
    Serial.print("Tempi=");
  Serial.print(helloTime);
  Serial.print("-");
  Serial.println(helloInterval);
  if(!helloHand.attached())  
  {
    helloHand.attach(HELLO_PIN);
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
  Wire.beginTransmission(9); // transmit to device #9
  if(s=="hi")
  {
    Wire.write("h");// sends 1 byte
  }
  if(s=="offer")
  {
    Wire.write("o");// sends 1 byte
  }
  Wire.endTransmission();    // stop transmitting
}

