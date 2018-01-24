// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int Degree;

char kind_of_data;
char SerialOutput[3];
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
  //open serial connection
  Serial.begin(115200);
   
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available() > 0) {
    kind_of_data = Serial.read();
    
    if(kind_of_data == 'L') {
      lcd.setCursor(0, 0);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
    if(kind_of_data == 'R') {
      lcd.setCursor(4, 0);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
    if(kind_of_data == 'F') {
      lcd.setCursor(8, 0);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
    if(kind_of_data == 'B') {
      lcd.setCursor(0, 1);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
    if(kind_of_data == 'A') {
      lcd.setCursor(4, 1);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
    if(kind_of_data == 'C') {
      lcd.setCursor(8, 1);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      String myStr = String((char *)SerialOutput);
      lcd.print(myStr);
    }
  }
}
