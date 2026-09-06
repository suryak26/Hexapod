#include <Servo.h>

// Declare servo objects for each of the 18 servos
Servo coxa[6], femur[6], tibia[6];

// Pin assignments for each leg's servos
int coxaPins[6] = {19, 25, 31, 37, 43, 49};
int femurPins[6] = {21, 27, 33, 39, 45, 51};
int tibiaPins[6] = {23, 29, 35, 41, 47, 53};

// Gait parameters
int liftHeight = 120;     // Femur and tibia lift angle
int swingForward = 70;   // Coxa forward swing angle
int groundContact = 90;  // Ground contact position (neutral)

// Speed of movement (adjust to control speed of gait)
int delayTime = 1000;     // Milliseconds delay between movements

// Function to move a specific leg (Coxa, Femur, Tibia) to specific angles
void moveLeg(int leg, int coxaAngle, int femurAngle, int tibiaAngle) {
  coxa[leg].write(coxaAngle);
  femur[leg].write(femurAngle);
  tibia[leg].write(tibiaAngle);
}

// Setup function to initialize servos and attach them to pins
void setup() {
  for (int i = 0; i < 6; i++) {
    coxa[i].attach(coxaPins[i]);
    femur[i].attach(femurPins[i]);
    tibia[i].attach(tibiaPins[i]);
    
    // Initialize all servos to neutral (ground contact position)
    moveLeg(i, groundContact, 90, 90);
  }
  delay(10000);  // Wait 1 second to stabilize before starting the gait
}

// Function to perform the diagonal tripod gait cycle
void tripodGaitCycle() {
  // Group A: Legs 2, 4, 6 (Middle-left, Front-right, Back-right)
  // Lift and swing forward Group B: Legs 5, 1, 3 (Middle-right, Front-left, Back-left)
  moveLeg(5, swingForward, liftHeight, liftHeight); // Leg 5
  moveLeg(1, swingForward, liftHeight, liftHeight); // Leg 1
  moveLeg(3, swingForward, liftHeight, liftHeight); // Leg 3
  delay(delayTime);
  
  // Place Group B back on the ground
  moveLeg(5, groundContact, 90, 90);  // Leg 5
  moveLeg(1, groundContact, 90, 90);  // Leg 1
  moveLeg(3, groundContact, 90, 90);  // Leg 3
  delay(delayTime);
  
  // Group B: Legs 5, 1, 3 (Middle-right, Front-left, Back-left)
  // Lift and swing forward Group A: Legs 2, 4, 6 (Middle-left, Front-right, Back-right)
  moveLeg(2, swingForward, liftHeight, liftHeight); // Leg 2
  moveLeg(4, swingForward, liftHeight, liftHeight); // Leg 4
  moveLeg(6, swingForward, liftHeight, liftHeight); // Leg 6
  delay(delayTime);
  
  // Place Group A back on the ground
  moveLeg(2, groundContact, 90, 90);  // Leg 2
  moveLeg(4, groundContact, 90, 90);  // Leg 4
  moveLeg(6, groundContact, 90, 90);  // Leg 6
  delay(delayTime);
}

void loop() {
  // Continuously perform the diagonal tripod gait
  tripodGaitCycle();
}
