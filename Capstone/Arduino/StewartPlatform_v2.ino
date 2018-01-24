#include <Servo.h>
#include <math.h>

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
const float L1 = 45, L2 = 95, z_home = 90;

int i;

String printString;

int Degree;

char kind_of_data;
char SerialOutput[3];

// Output pins for the servo motors, in order
const int servoPins[6] = {9,3,5,11,6,10};
Servo servo[6];







void setup() {
  Serial.begin(9600);
  Serial.println("START");
}






void loop() {
  
  printString = "";

  // Change the orientation matrix to the desired values
  // new_plat_coords = the positions of the platform joints after moving to the desired orientation - first index = joint number, second index = x/y/z
  // dl = x/y/z distances from the base joints to the platform joints
  // dl2 = hypotenuse of the dl's
  // L/M/N = lengths 
  // servo_angle = the angles for each servo motor
  float sway = 30, surge = -30, heave = 20, pitch = degrees(0.174532925), roll = degrees(0), yaw = 0;
  
  static float orientation[6] = {sway, surge, heave, pitch, roll, yaw}, new_plat_coords[6][3], dl[6][3], dl2[6], L[6], M[6], N[6], servo_angle[6];

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

    printString += String(i+1) + ". " + String(servo_angle[i]) + "_" + isnan(servo_angle[i]) + "\t";
    
  }
  
  printString += "newPlatX: " + String(new_plat_coords[i][0]) + "\t";
  printString += "newPlatY: " + String(new_plat_coords[i][1]) + "\t";
  printString += "newPlatZ: " + String(new_plat_coords[i][2]) + "\t";
  
  printString += "L: " + String(L[i]) + "\t";
  printString += "M: " + String(M[i]) + "\t";
  printString += "N: " + String(N[i]) + "\t";
  printString += "dl2: " + String(dl2[i]) + "\t";
  
  Serial.print(printString);
  Serial.println("");

  //anglesToServos(servo_angle[0],servo_angle[1],servo_angle[2],servo_angle[3],servo_angle[4],servo_angle[5]);
  
}
/*
void orientationToAngles() {

  static float orientation[6] = {sway, surge, heave, pitch, roll, yaw};
  
  static float new_plat_coords[6][3], dl[6][3], dl2[6], L[6], M[6], N[6], servo_angle[6];

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
    
  }
  
}
*/

void anglesToServos(float angle1, float angle2, float angle3, float angle4, float angle5, float angle6) {

  boolean anglesVerified = true;
  static float servo_angle[6] = {angle1, angle2, angle3, angle4, angle5, angle6};

  // Check to make sure that all servo angles are valid before trying to set any of the motor positions 
  for(i = 0; i < 6; i++){
    if(isnan(servo_angle[i])) {
      anglesVerified = false;
    }
  }

  if(anglesVerified){
    for(i = 0; i < 6; i++){
  
      // Constrain the angles to the operational limits
      if(servo_angle[i] < 0){
        servo_angle[i] = 0;
      } else if(servo_angle[i] > 180){
        servo_angle[i] = 180;
      }
  
      // Set the servo motor angle
      servo[i].write(servo_angle[i]);
    }
  }
}






