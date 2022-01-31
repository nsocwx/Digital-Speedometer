// nsocwx 2022
// @nsocwx
// Digital speedometer
// VSS on car connects to pin 3
// see wiring diagram here https://crcit.net/c/905a34e381de435d8df352c393757068

// Speedometer stepper control for X27
// Odometer display for SSD1306 OLED
// Mileage has a memory limit of 1,073,741 due to limitations of the 'long' datatype

/*****************************/
//includes
#include <SPI.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include <SwitecX25.h>
#include <util/atomic.h>
#include <EEPROM.h>
/*****************************/
//definitions
//A4 and A5 used for OLED screen
//screen
#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c display;
//dial
#define STEPS 945 // 315 degrees of range = 315x3 steps = 945 steps
SwitecX25 speedo(STEPS, 4, 5, 6, 7); //create object for speedometer with pins
//vss
#define VSSPin 3 //define pin to listen for VSS signal on
/*****************************/
//variables
unsigned int cycles; //number of times loop has run
unsigned int position; //set position for stepper motor
volatile unsigned int PULSE;//counter for pulses per mile
unsigned long mileage;//miles the vehicle has driven
unsigned int vss,VSS; //variable to store vehicle speed
unsigned int delta; //holds the delta time
unsigned long timeNew; //variable to store current check time
unsigned long timeOld;  //variable to store previous check time
const unsigned int pulsesPerMile = 4000;//variable to store vss ppm
const unsigned int convertMph = 36000/pulsesPerMile;
unsigned long mileageWhole; //mileage output to screen before decimal
unsigned int mileageTenth; //mileage output to screen after decimal
int out[] = {0,75,155,245,325,408,486,568,658,740,810}; //stepper positions for each 10mph
int in[] = {0,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000}; //each 10mph x 100
/*****************************/
void setup() {
  Serial.begin(115200);
  //display
  EEPROM.get(0, mileage);  
  display.begin(&Adafruit128x64, I2C_ADDRESS);
  display.setFont(lcdnums12x16);
  display.clear();
  //dial
  speedo.zero();
  //vss
  attachInterrupt(digitalPinToInterrupt(VSSPin),VSSCount,FALLING);//create interrupt on VSSPin to run VSSCount() on each pulse
  timeOld = millis();
  delay(2000);
}
/*****************************/
void loop() {
  cycles++;//increment cycle count
  //run this code every 500 cylces (half second)
  if(cycles >= 100){
    cycles = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
      vss = readVss(); //read VSS inside atomic block for accuracy
    }
    position = multiMap(vss, in, out, 11); //map stepper position

    speedo.setPosition(position); //set speedo to mapped position
    display.setCursor(17,0);//centers text
    //code to breakdown numbers for before and after decimal
    mileageWhole = mileage/pulsesPerMile;
    mileageTenth = (mileage*10/pulsesPerMile)%10;
    //outputs leading zeros to screen
    if(mileageWhole < 10){
      display.print("00000");
    }
    else if(mileageWhole < 100){
      display.print("0000");
    }
    else if(mileageWhole < 1000){
      display.print("000");
    }
    else if(mileageWhole < 10000){
      display.print("00");
    }
    else if(mileageWhole < 100000){
      display.print("0");
    }
    display.print(mileageWhole);//before decimal
    display.print("."); 
    display.println(mileageTenth);//after decimal
    if(vss <= 0){//if the vehicle is stopped, update EEPROM mileage
      EEPROM.put(0,mileage);
    }  
    Serial.print((float)vss/100,2);
    Serial.print(" ");
    Serial.println(position);
   }
  //end of 500 cycle code statement
  speedo.update();//move stepper to target location
  delay(5);
}
/*****************************/
void VSSCount() //runs when VSSPin sees a rising signal
{
  PULSE++;//increment pulse counter by 1;
}
/*****************************/
unsigned int readVss()//runs when called
{
  unsigned int result;
  timeNew = millis();
  delta = (timeNew-timeOld);
  result = 10000/delta*PULSE*convertMph;
  timeOld = timeNew;
  mileage = mileage + PULSE;
  PULSE = 0;
  return result;
}
unsigned int multiMap(int value, int* _in, int* _out, uint8_t size)
{
    // take care the value is within range
    if (value <= _in[0]) return _out[0];
    if (value >= _in[size-1]) return _out[size-1];

    // search right interval
    uint8_t pos = 1;  // _in[0] already tested
    while(value > _in[pos]) pos++;

    // this will handle all exact "points" in the _in array
    if (value == _in[pos]) return _out[pos];

    // interpolate in the right segment for the rest
    uint16_t result = map(value,_in[pos-1],_in[pos],_out[pos-1],_out[pos]);
    return result;
}
