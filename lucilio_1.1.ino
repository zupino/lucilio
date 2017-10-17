/*
08.10.2017 Now with simple sounds
*/

#include <Adafruit_NeoPixel.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <string.h>

// For buttons SW de-bouncing
#include <Bounce.h>

// For the rotary encoder
#include <Encoder.h>

// For the audio shield
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// PIN is the controller for the LED strip
#define PIN         0
#define N_LEDS      8

// Buttons
#define BLUE_PIN    5
#define BLACK_PIN   4
#define RED_PIN     3
#define GREEN_PIN   2
#define YELLOW_PIN  1

Bounce blueB = Bounce(BLUE_PIN, 10);
Bounce blackB = Bounce(BLACK_PIN, 10);
Bounce redB = Bounce(RED_PIN, 10);
Bounce greenB = Bounce(GREEN_PIN, 10);
Bounce yellowB = Bounce(YELLOW_PIN, 10);

// This is the folder name for the audio file
// I want to add a knob to change the value of the 
// dolfer and read .WAV file from different SD card
// location
char folder[80] = "0";
char tmp[80] = "";
char sound[80] = "";
uint32_t color = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN);

// Rotary encoder
Encoder knob(16, 20);
long rotPos = 0;
long newRotPos = 0;
int fileNumber = 0;
 
void setup() {

  Serial.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.85);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    Serial.println("Unable to access the SD card");
    delay(500);
  }
  
  pinMode(BLACK_PIN, INPUT_PULLUP);
  pinMode(RED_PIN, INPUT_PULLUP);
  pinMode(GREEN_PIN, INPUT_PULLUP);
  pinMode(YELLOW_PIN, INPUT_PULLUP);
  pinMode(BLUE_PIN, INPUT_PULLUP);

  // In the Teensy low power guide, they recommend turning unused pins to OUTPUT
  /*
  int i; 
  for (i=0; i < 34; i++) {
     if (i > 5 && i != PIN) {
         pinMode(i, OUTPUT);
     }
  }
  */

  
  
  strip.begin();
  strip.setBrightness(75);

  changeAllColor( 255, 255, 255 );
  
}
 
void loop() {

  // Check the rotary value
  newRotPos = knob.read();
  if ( (newRotPos != rotPos)  ) {
    // Serial.print("Rotary value = ");
    // Serial.print(newRotPos);
    // Serial.println();
    
    
    
    if( (newRotPos % 4) == 0 ) {
      if( rotPos < newRotPos ){
        fileNumber++;
        fileNumber = fileNumber % 3;
      } else {
        fileNumber--;
        fileNumber = fileNumber % 3;
        if( fileNumber < 0){
          fileNumber = fileNumber * -1;  
        }
      }
      
      sprintf(folder, "%d", fileNumber);
      Serial.print("Changing file source: ");
      Serial.print(folder);
      Serial.println();
    }

    rotPos = newRotPos;
    
  }
  

  if( blackB.update() ){
    if( blackB.fallingEdge() ) {
      // white ( color == 4 )

      changeAllColor( 255, 255, 255 );

      strcat(tmp, folder);
      playFile( strcat(tmp,"_1.WAV") );
      strcpy(tmp,"");
    }
  }
  
  if( redB.update() ){
    if( redB.fallingEdge() ) {
      // red ( color == 3 )

      changeAllColor( 255, 0, 0 );

      strcat(tmp, folder);
      playFile( strcat(tmp,"_2.WAV") );
      strcpy(tmp,"");
    }
  }  

   if( yellowB.update() ){
    if( yellowB.fallingEdge() ) {
      // yellow ( color == 1 )

      changeAllColor( 255, 255, 0 );
      
      strcat(tmp, folder);
      playFile( strcat(tmp,"_3.WAV") );
      strcpy(tmp,"");
    }
  }   

   if( greenB.update() ){
    if( greenB.fallingEdge() ) {
      // green ( color == 2 )

      changeAllColor( 0, 255, 0 );

      strcat(tmp, folder);
      playFile( strcat(tmp,"_4.WAV") );
      strcpy(tmp,"");
    }
  }  

   if( blueB.update() ){
    if( blueB.fallingEdge() ) {
      // blue ( color == 5 )

      changeAllColor( 0, 0, 255 );

      strcat(tmp, folder);
      playFile( strcat(tmp,"_5.WAV") );
      strcpy(tmp,"");
    }
  }  
  
}

void playFile(const char *filename)
{
  sprintf(sound, folder);
  sprintf(sound, filename);

  Serial.print("Playing file: ");
  Serial.println(sound);


  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(sound);

  sprintf(sound, "");
  // A brief delay for the library read WAV info
  delay(5);

}

// Change the color of all the pixel
static void changeAllColor(uint32_t red, uint32_t green, uint32_t blue) {

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, red, green, blue);
    strip.show();
  }
  
}

/* 
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(5);
  }
}
*/


