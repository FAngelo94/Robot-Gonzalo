#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <PinChangeInt.h>
#include <Wire.h>

#define FIRST_MODE 0
#define TRACK 0
#define VOLUME 1
#define LAST_MODE 1
#define SHDN_GPIO1 A2

unsigned char volume = 40;
char track[10]="1.MP3";

boolean playing = false;

SdFat sd;
SdFile file;
SFEMP3Shield MP3player;

void setup() {
  Serial.begin(9600);
  byte result;
  Serial.println(F("Lilypad MP3 Player"));
  Serial.print(F("Free RAM = "));
  Serial.println(FreeRam(), DEC);
  // Initialize the SD card:
  Serial.println(F("Initializing SD card... "));

  result = sd.begin(SD_SEL, SPI_HALF_SPEED);

  if (result != 1)
    Serial.println(F("error, halting"));
  else 
    Serial.println(F("OK"));
 
  //Initialize the MP3 chip:
  Serial.println(F("Initializing MP3 chip... "));
  result = MP3player.begin();

  // Check result, 0 and 6 are OK:
  if((result != 0) && (result != 6))
  {
      Serial.print(F("error "));
      Serial.println(result);
  }
  else
    Serial.println(F("OK"));
  
  // Get initial track:
  sd.chdir("/",true); // Index beginning of root directory
  Serial.print(F("current track: "));
  Serial.println(track);
  
  // Set initial volume (same for both left and right channels)
  MP3player.setVolume(volume, volume);
  // Turn on amplifier chip:
  digitalWrite(SHDN_GPIO1, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0)
  {
    String leggi=Serial.readString();
    if(leggi=="play")
      startPlaying();
    if(leggi=="stop")
      stopPlaying();
    Serial.println("PLAY");
  }
  delay(3000);
}

void startPlaying()
{
  int result;
  Serial.print(F("playing "));
  Serial.print(track);
  Serial.print(F("..."));
  result = MP3player.playMP3(track);
  Serial.print(F(" result "));
  Serial.println(result);  
}
void stopPlaying()
{
  Serial.println(F("stopping playback"));
  MP3player.stopTrack();
}
