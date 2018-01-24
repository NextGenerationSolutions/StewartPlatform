// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int Degree;

char kind_of_data;
char SerialOutput[3];
int rawForces[6];
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
    
    if(kind_of_data == 'X') {
      Serial.readBytesUntil('Y', SerialOutput, 40);
      rawForce[0] = atoi(SerialOutput);
      rawForce[0] = constrain(rawForce[0], 100000000, -100000000); //need max X, need min X
    }
    if(kind_of_data == 'Y') {
      Serial.readBytesUntil('Z', SerialOutput, 40);
      rawForce[1] = atoi(SerialOutput); 
      rawForce[1] = constrain(rawForce[1], 100000000, -100000000); //need max Y, need min Y
    }
    if(kind_of_data == 'Z') {
      Serial.readBytesUntil('P', SerialOutput, 40);
      rawForce[2] = atoi(SerialOutput); 
      rawForce[2] = constrain(rawForce[2], 250000000, -250000000); //need max Z, need min Z
    }
    if(kind_of_data == 'P') {
      lcd.setCursor(0, 1);
      Serial.readBytesUntil('R', SerialOutput, 40);
      rawForce[3] = atoi(SerialOutput); 
      rawForce[3] = constrain(rawForce[3], 1015704118, -1015704118); //need max Pitch, need min Pitch
    }
    if(kind_of_data == 'R') {
      lcd.setCursor(4, 1);
      Serial.readBytesUntil('R', SerialOutput, 40);
      rawForce[4] = atoi(SerialOutput); 
      rawForce[4] = constrain(rawForce[4], 1500000000, -1500000000); //need max Roll, need min Roll
    }
    if(kind_of_data == 'W') {
      lcd.setCursor(8, 1);
      Serial.readBytesUntil('\r', SerialOutput, 40);
      rawForce[5] = atoi(SerialOutput); 
      rawForce[5] = constrain(rawForce[5], 2147483647, -2147483647); //need max Yaw, need min Yaw
    }
  }
}


