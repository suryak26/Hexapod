#include <Servo.h>

// Define servo objects for each leg's Coxa, Femur, and Tibia
Servo coxa1, femur1, tibia1;
Servo coxa2, femur2, tibia2;
Servo coxa3, femur3, tibia3;
Servo coxa4, femur4, tibia4;
Servo coxa5, femur5, tibia5;
Servo coxa6, femur6, tibia6;

// Pin definitions from your image
const int COXA1_PIN = 23, FEMUR1_PIN = 21, TIBIA1_PIN = 19;
const int COXA2_PIN = 25, FEMUR2_PIN = 27, TIBIA2_PIN = 29;
const int COXA3_PIN = 35, FEMUR3_PIN = 31, TIBIA3_PIN = 22;
const int COXA4_PIN = 41, FEMUR4_PIN = 39, TIBIA4_PIN = 37;
const int COXA5_PIN = 43, FEMUR5_PIN = 45, TIBIA5_PIN = 47;
const int COXA6_PIN = 49, FEMUR6_PIN = 51, TIBIA6_PIN = 53;

// Define movement angles for each servo
const int COXA_FORWARD = 110;
const int COXA_BACKWARD = 70;
const int FEMUR_LIFT = 30;
const int FEMUR_LOWER = 90;
const int TIBIA_EXTEND = 90;
const int TIBIA_RETRACT = 60;

// Lift, move, and lower leg (Set of 3 servos: Coxa, Femur, Tibia)
void liftLeg(Servo& femur, Servo& tibia) {
  femur.write(FEMUR_LIFT);
  tibia.write(TIBIA_RETRACT);
}

void lowerLeg(Servo& femur, Servo& tibia) {
  femur.write(FEMUR_LOWER);
  tibia.write(TIBIA_EXTEND);
}

void moveLegForward(Servo& coxa) {
  coxa.write(COXA_FORWARD);
}

void moveLegBackward(Servo& coxa) {
  coxa.write(COXA_BACKWARD);
}

void setup() {
  // Attach each servo to its corresponding pin

  coxa1.attach(COXA1_PIN); femur1.attach(FEMUR1_PIN); tibia1.attach(TIBIA1_PIN);
  coxa2.attach(COXA2_PIN); femur2.attach(FEMUR2_PIN); tibia2.attach(TIBIA2_PIN);
  coxa3.attach(COXA3_PIN); femur3.attach(FEMUR3_PIN); tibia3.attach(TIBIA3_PIN);
  coxa4.attach(COXA4_PIN); femur4.attach(FEMUR4_PIN); tibia4.attach(TIBIA4_PIN);
  coxa5.attach(COXA5_PIN); femur5.attach(FEMUR5_PIN); tibia5.attach(TIBIA5_PIN);
  coxa6.attach(COXA6_PIN); femur6.attach(FEMUR6_PIN); tibia6.attach(TIBIA6_PIN);
  coxa1.write(90); femur1.write(90); tibia1.write(90);
  coxa2.write(90); femur2.write(90); tibia2.write(90);
  coxa3.write(90); femur3.write(90); tibia3.write(90);
  coxa4.write(90); femur4.write(90); tibia4.write(90);
  coxa5.write(90); femur5.write(90); tibia5.write(90);
  coxa6.write(90); femur6.write(90); tibia6.write(90);
  delay(5000);
}

void tripodGait() {
  // Tripod sets: Set A (Legs 1, 3, 5) and Set B (Legs 2, 4, 6)

  // Step 1: Lift Set A (Legs 1, 3, 5), move forward, and lower them
  liftLeg(femur1, tibia1); liftLeg(femur3, tibia3); liftLeg(femur5, tibia5);
  delay(300);
  
  moveLegForward(coxa1); moveLegForward(coxa3); moveLegForward(coxa5);
  delay(300);
  
  lowerLeg(femur1, tibia1); lowerLeg(femur3, tibia3); lowerLeg(femur5, tibia5);
  delay(300);

  // Step 2: Set B (Legs 2, 4, 6) stays on the ground and moves backward
  moveLegBackward(coxa2); moveLegBackward(coxa4); moveLegBackward(coxa6);
  delay(300);

  // Step 3: Lift Set B (Legs 2, 4, 6), move forward, and lower them
  liftLeg(femur2, tibia2); liftLeg(femur4, tibia4); liftLeg(femur6, tibia6);
  delay(300);
  
  moveLegForward(coxa2); moveLegForward(coxa4); moveLegForward(coxa6);
  delay(300);
  
  lowerLeg(femur2, tibia2); lowerLeg(femur4, tibia4); lowerLeg(femur6, tibia6);
  delay(300);

  // Step 4: Set A (Legs 1, 3, 5) stays on the ground and moves backward
  moveLegBackward(coxa1); moveLegBackward(coxa3); moveLegBackward(coxa5);
  delay(300);
}

void loop() {
  tripodGait();  // Continuously perform tripod gait
}
