/*
 * Author: Elijah Solley
 * Created: 7/27/19
 * 
 */

#include <Keypad.h> //The keypad and LCD i2c libraries
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27 //defining the LCD pins
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define sensor A2

LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//Keypad Map
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3

char keypressed = '1', previous;

// Setup Keypad
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
void setup()
           {
            Serial.begin(9600);
            lcd.begin (16,2);
            lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
            lcd.setBacklight(HIGH);
            lcd.home ();
           }

void loop()
{
    
         lcd.print("Press a key.");
         lcd.setCursor(0,1);
         lcd.print("# to enter.");
         while(1) {
             keypressed = myKeypad.getKey();
             if (keypressed == '1' | keypressed == '2' | keypressed == '3' | keypressed == '4')
               {
                    lcd.clear();
                    lcd.print(keypressed);
                    previous = keypressed;
               }
             else if (keypressed == '#')
               {
                    lcd.clear();
                    lcd.print("Selected " + previous);
                    Serial.write(previous);
                    break;
               }
             else 
               {}
             
         }
}
