// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <math.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int Degree;

char kind_of_data;
char SerialOutput[3];
float rawForce[6], servo_angle[6];

// Base joint coordinates - first index = joint number, second index = x/y/z
const float base_coords[6][3] = 
{{61.28355545, 51.42300877, 0},   
{75.17540966, 27.36161147, 0},   
{13.89185421, -78.78462024, 0},   
{-13.89185421, -78.78462024, 0},   
{-75.17540966, 27.36161147, 0},   
{-61.28355545, 51.42300877, 0}};

// Platform joint coordinates (default home-positions) - first index = joint number, second index = x/y
const float plat_coords[6][2] = 
{{35.35533906, 35.35533906},
{48.29629131, 12.94095226},
{12.94095226, -48.29629131},
{-12.94095226, -48.29629131},
{-48.29629131, 12.94095226},
{-35.35533906, 35.35533906}};

// Theta_S values (angle that the servo arm goes up/down from relative to the servo)
const int theta_s[6] = {180, -120, 60, 120, -60, 0};

// L1 = Servo arm length
// L2 = Connecting arm length
// z_home = default z height
const float L1 = 45, L2 = 95, z_home = 90, servo_min = 16.56, servo_max = 163.44;

int i;

String printString;

// Output pins for the servo motors, in order
const int servoPins[6] = {9,3,5,11,6,10};
Servo servo[6];

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
      rawForce[0] = constrain(rawForce[0], 27215708, 4237935081); //testing revealed these max X, min X
      rawForce[0] = map(rawForce[0], 0, 4294967295, -50, 50);
    }
    if(kind_of_data == 'Y') {
      Serial.readBytesUntil('Z', SerialOutput, 40);
      rawForce[1] = atoi(SerialOutput); 
      rawForce[1] = constrain(rawForce[1], 23822044, 4254634074); //testing revealed these max Y, min Y
      rawForce[1] = map(rawForce[1], 0, 4294967295, -50, 50);
    }
    if(kind_of_data == 'Z') {
      Serial.readBytesUntil('P', SerialOutput, 40);
      rawForce[2] = atoi(SerialOutput); 
      rawForce[2] = constrain(rawForce[2], 128219746, 4217812680); //testing revealed these max Z, min Z
      rawForce[2] = map(rawForce[2], 0, 4294967295, -50, 50);
    }
    if(kind_of_data == 'P') {
      Serial.readBytesUntil('R', SerialOutput, 40);
      rawForce[3] = atoi(SerialOutput); 
      rawForce[3] = constrain(rawForce[3], 352516354, 3942450943); //testing revealed these max Pitch, min Pitch
      rawForce[3] = map(rawForce[3], 0, 4294967295, -0.523598776, 0.523598776);
    }
    if(kind_of_data == 'R') {
      Serial.readBytesUntil('R', SerialOutput, 40);
      rawForce[4] = atoi(SerialOutput); 
      rawForce[4] = constrain(rawForce[4], 412309474, 3857523307); //testing revealed these d max Roll, min Roll
      rawForce[4] = map(rawForce[4], 0, 4294967295, -0.523598776, 0.523598776);
    }
    if(kind_of_data == 'W') {
      Serial.readBytesUntil('\r', SerialOutput, 40);
      rawForce[5] = atoi(SerialOutput); 
      rawForce[5] = constrain(rawForce[5], 425905475, 3941076030); //testing revealed these max Yaw, min Yaw
      rawForce[5] = map(rawForce[5], 0, 4294967295, -0.523598776, 0.523598776);
    }
    arrayToAngles(rawForce[0], rawForce[1], rawForce[2], rawForce[3], rawForce[4], rawForce[5]);
  }  
}


void arrayToAngles(float sway, float surge, float heave, float pitch, float roll, float yaw){
  // Change the orientation matrix to the desired values
  // new_plat_coords = the positions of the platform joints after moving to the desired orientation - first index = joint number, second index = x/y/z
  // dl = x/y/z distances from the base joints to the platform joints
  // dl2 = hypotenuse of the dl's
  // L/M/N = lengths 
  // servo_angle = the angles for each servo motor
  //float sway = 30, surge = -30, heave = 20, pitch = degrees(0.174532925), roll = degrees(0), yaw = 0;
  
  //static float orientation[6] = {sway, surge, heave, pitch, roll, yaw}, 
  static float new_plat_coords[6][3], dl[6][3], dl2[6], L[6], M[6], N[6];

  for(i = 0; i < 6; i++){
    
    // New plat coords - x/y/z
    new_plat_coords[i][0] = plat_coords[i][0]*cos(radians(yaw))*cos(radians(roll)) + plat_coords[i][1]*(sin(radians(pitch))*sin(radians(roll))*cos(radians(roll))-cos(radians(pitch))*sin(radians(yaw))) + sway;
    new_plat_coords[i][1] = plat_coords[i][0]*cos(radians(roll))*sin(radians(yaw)) + plat_coords[i][1]*(cos(radians(pitch))*cos(radians(yaw))+sin(radians(pitch))*sin(radians(roll))*sin(radians(yaw))) + surge;
    new_plat_coords[i][2] = (-1*plat_coords[i][0])*sin(radians(roll)) + plat_coords[i][1]*sin(radians(pitch))*cos(radians(roll)) + z_home + heave;

    dl[i][0] = base_coords[i][0] - new_plat_coords[i][0];
    dl[i][1] = base_coords[i][1] - new_plat_coords[i][1];
    dl[i][2] = base_coords[i][2] - new_plat_coords[i][2];

    dl2[i] = sqrt(dl[i][0]*dl[i][0] + dl[i][1]*dl[i][1] + dl[i][2]*dl[i][2]);

    L[i] = dl2[i]*dl2[i] - (L2*L2 - L1*L1);
    M[i] = 2*L1*(new_plat_coords[i][2] - base_coords[i][2]);
    N[i] = 2*L1*(cos(radians(theta_s[i])) * (new_plat_coords[i][0] - base_coords[i][0]) + sin(radians(theta_s[i]))*(new_plat_coords[i][1] - base_coords[i][1]));
    
    servo_angle[i] = degrees(asin(L[i]/sqrt(M[i]*M[i] + N[i]*N[i])) - atan(N[i]/M[i]));
    
    // Servo_angle values currently use the horizontal as the origin
    // Since motor orientation alternates, the calculated servo_angle value 
    // needs to either be added/subtracted from 90 to get the correct servo angle.
    if(i%2 == 1){
      servo_angle[i] = 90 + servo_angle[i];
    } else {
      servo_angle[i] = 90 - servo_angle[i];
    }
    anglesToServo(servo_angle);
  }
}

void anglesToServo(float servoAngle [6]){
   boolean anglesVerified = true;
//  static float servoAngle[6] = {angle1, angle2, angle3, angle4, angle5, angle6};

  // Check to make sure that all servo angles are valid before trying to set any of the motor positions 
  for(i = 0; i < 6; i++){
    if(isnan(servoAngle[i])) {
      anglesVerified = false;
    }
  }

  if(anglesVerified){
    for(i = 0; i < 6; i++){
  
      // Constrain the angles to the operational limits
      servoAngle[i] = constrain(servoAngle[i], servo_min, servo_max);
  
      // Set the servo motor angle
      servo[i].write(servoAngle[i]);
    }
  }
}

