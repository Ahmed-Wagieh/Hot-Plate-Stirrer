/*This version works with hot_plate_stirrer_5.mlapp
 * in this version I added the ability to auto scroll the lcd
 */
#include "max6675.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define SO  9
#define CS  10
#define CLK 13
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

volatile uint16_t temp_C;
volatile uint16_t ref_temp = 0;
volatile uint16_t alpha = 5000;
bool rising = true; // states wither the temperature is rising or falling
uint8_t stirrer_heater = 3;
volatile bool heater = 0, stirrer = 1;
int i = 0;
char* team1 = "  Team members:   Ahmed Atef * Ahmed Wagieh * Hassan Khaled * Hussien Elkassar * Khaled Osama * Abdelrahman Atef * Omar Gamal * Mohamed Alawady * Mohamed Osama * Mahmoud Aladawy     under supervision of Dr Saber Abdrabbo.      ";
MAX6675 thermocouple(CLK, CS, SO);
//volatile uint32_t count = 0;
void zero_detect(){
  if(heater & ((temp_C <= ref_temp*0.9 && rising) || (temp_C <= ref_temp*1.1 && !rising))){
    delayMicroseconds(alpha);
    digitalWrite(4, HIGH);
    delayMicroseconds(100);
    digitalWrite(4, LOW);
  }
}
void setup() {
  Serial.begin(57600);
  attachInterrupt(digitalPinToInterrupt(3), zero_detect, RISING);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  analogWrite(5, 100);
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();//To Power ON the back light
  lcd.clear();//Clean the screen
  lcd.setCursor(0,0); 
  lcd.print("     Welcome ");
  lcd.setCursor(0,1);
  lcd.print("Mechatronics 150 ");
  delay(1000);
  //digitalWrite(4, HIGH);

}

void loop() {
  temp_C = 100 * thermocouple.readCelsius();
  Serial.write((uint8_t)(temp_C));
  Serial.write((uint8_t)(temp_C >> 8));
  //while(!Serial.available() ==2);
  if (Serial.available() >= 2){
    ref_temp = Serial.read()*100;
    stirrer_heater = Serial.read();
  }
  stirrer = stirrer_heater & 1;
  heater = stirrer_heater & 2;
  if(stirrer)analogWrite(5, 100);
  else analogWrite(5, 0);
  if(i/4 <= 210)i++;
  else i=0;
  if(i%4 == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    for(int j=i/4;j<16+i/4;j++)
    lcd.print(team1[j]);
    lcd.setCursor(1, 1);
    lcd.print("Temp. -> ");
    lcd.print(temp_C/100.0);
    //delay(100);
  }
  if (ref_temp <= 50)alpha = 4000;
  else if (ref_temp <= 100)alpha = 3000;
  else if (ref_temp <= 150)alpha = 2500;
  else if (ref_temp <= 200)alpha = 2000;
  if(rising && temp_C >= ref_temp*1.05)rising = false;
  else if(!rising && temp_C <= ref_temp*0.95)rising = true;
  delay(200);  // The thermo couple will not work if this value is decrease below 200
}
