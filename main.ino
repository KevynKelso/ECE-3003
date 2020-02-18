#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <LSM303.h>
#include <TinyGPS.h>
#include <Servo.h>
#include "ESC.h"
/**********************************************************/
#define DISPLAY_TIME 200
#define CM_CONVERSION 2.54
#define trigPin 2
#define echoPin 3
#define SPEED_MIN (1000)
#define SPEED_MAX (2000)  

char line0[21];
char line1[21];

char cm_str[8];
char in_str[8];
char head_str[8];
char lat_str[12];
char lon_str[12];

double distanceCm;
double distanceIn;
float lat,lon;
int oESC;
int pos = 0;
/********************* OBJECTS ****************************/
LiquidCrystal_I2C lcd(0x27,16,2); 
LSM303 compass;
TinyGPS gps;
Servo steer;
ESC myESC (9, SPEED_MIN, SPEED_MAX, 500);
/*********************************************************/
void setup()
{  
  //initialize
  Serial.begin(57600);
  Wire.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.init();
  compass.init();
  lcd.backlight();
  compass.enableDefault();
  
  //compass calibration
//min: { -2827,  -1951,  -2519}    max: { +2349,  +2582,  +1997}
  compass.m_min = (LSM303::vector<int16_t>){-2827, -1951, -2519};
  compass.m_max = (LSM303::vector<int16_t>){+2349, +2582, +1997};
  Serial.println("The GPS Received Signal:");
  Serial1.begin(9600); // connect gps sensor
  steer.attach(8);
  myESC.arm();
}
/*********************************************************/
void loop() 
{
 for (oESC = SPEED_MIN; oESC <= SPEED_MAX; oESC += 1) {  // goes from 1000 microseconds to 2000 microseconds
    myESC.speed(oESC);                                    // tell ESC to go to the oESC speed value
    delay(10);                                            // waits 10ms for the ESC to reach speed
  }
  delay(1000);
  for (oESC = SPEED_MAX; oESC >= SPEED_MIN; oESC -= 1) {  // goes from 2000 microseconds to 1000 microseconds
    myESC.speed(oESC);                                    // tell ESC to go to the oESC speed value
    delay(10);                                            // waits 10ms for the ESC to reach speed  
   }
  delay(5000);
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
  // in steps of 1 degree
    steer.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    steer.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
//  for (int i = 0; i < DISPLAY_TIME; i++) {
//    updateDisplay();
//    distanceCm = getUltrasonicCm();
//    distanceIn = cmToIn(distanceCm);
//    
//    dtostrf(distanceCm,4,2,cm_str);
//    dtostrf(distanceIn,4,2,in_str);
//    sprintf(line0, "Cm:~%-11s", cm_str);
//    sprintf(line1, "In:~%-11s", in_str);
//  }
//  for (int i = 0; i < DISPLAY_TIME; i++) {
//    updateDisplay();
//    compass.read();
//    float heading = compass.heading();
//    dtostrf(heading,4,2,head_str);
//    sprintf(line0, "Heading:~%-7s",head_str);
//    sprintf(line1, "            ");
//  }

//  for (int i = 0; i < DISPLAY_TIME; i++) {
//    updateDisplay();
// //   if (Serial1.available()){ // check for gps data
//      if (gps.encode(Serial1.read())){
//          gps.f_get_position(&lat,&lon); // get latitude and longitude
//          dtostrf(lat,4,6,lat_str);
//          dtostrf(lon,4,6,lon_str);
//          sprintf(line0, "Lat:%4.8s",lat_str);
//          sprintf(line1, "Lon:%4.8s",lon_str);
//   //   }
//    }
//  }
}
/********** FUNCTIONS ************************************/
double getUltrasonicCm() {
  double duration;
  double distanceCm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm= duration*0.034/2;
  return distanceCm;
}
double cmToIn(double distanceCm) {
  return distanceCm/CM_CONVERSION;
}
void updateDisplay() {
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}
/************************************************************/
