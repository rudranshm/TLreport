#include "Wire.h"
#include <MPU6050.h>
#include <NewPing.h>

MPU6050 mpu;


#define TRIG_PIN_FRONT 7
#define ECHO_PIN_FRONT 6
#define TRIG_PIN_LEFT A0
#define ECHO_PIN_LEFT A1
#define TRIG_PIN_RIGHT A2
#define ECHO_PIN_RIGHT A3
#define enA 8
#define in2 9
#define in1 10
#define in3 11
#define in4 12
#define enB 13
#define leftEncoderPin 1 
#define rightEncoderPin 2 

#define MAX_DISTANCE 100
#define MAX_SPEED 160
#define MAX_SPEED_OFFSET 40
#define COLL_DIST 10
#define TURN_DIST COLL_DIST + 5

int motorSpeed = 150; // Adjust this value as needed
int correctionFactor = 10; // Adjust this value as needed

NewPing frontUltrasonic(TRIG_PIN_FRONT, ECHO_PIN_FRONT, MAX_DISTANCE);
NewPing leftUltrasonic(TRIG_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing rightUltrasonic(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

volatile long leftEncoderCount = 0;
volatile long rightEncoderCount = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIG_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);
  pinMode(TRIG_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIG_PIN_RIGHT, OUTPUT);
  pinMode(ECHO_PIN_RIGHT, INPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(enB, OUTPUT);

   Wire.begin();
  mpu.initialize();

  pinMode(leftEncoderPin, INPUT_PULLUP);
  pinMode(rightEncoderPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(leftEncoderPin), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(rightEncoderPin), rightEncoderISR, RISING);
}

void loop()
{ delay(2000);
  
  float frontDistance, leftDistance, rightDistance;
  calculateDistances(frontDistance, leftDistance, rightDistance);

  Serial.print("Front Distance: ");
  Serial.println(frontDistance);
  Serial.print("Left Distance: ");
  Serial.println(leftDistance);
  Serial.print("Right Distance: ");
  Serial.println(rightDistance);
 delay(500);



  int16_t gyroX, gyroY, gyroZ;
  mpu.getRotation(&gyroX, &gyroY, &gyroZ);

  printGyroValues();
delay(500);

  float pitchAngle = atan2(gyroX, sqrt(gyroY * gyroY + gyroZ * gyroZ)) * 180.0 / M_PI;
  float rollAngle = atan2(gyroY, sqrt(gyroX * gyroX + gyroZ * gyroZ)) * 180.0 / M_PI;


  int motorSpeedLeft = motorSpeed;
  int motorSpeedRight = motorSpeed;

  // Calculate the correction factor based on pitch and roll
  int correction = (int)(correctionFactor * (rollAngle - pitchAngle));

  // Apply the correction to motor speeds
  motorSpeedLeft += correction;
  motorSpeedRight -= correction;
   motorSpeedLeft = constrain(motorSpeedLeft, 0, 255);
  motorSpeedRight = constrain(motorSpeedRight, 0, 255);

  // Control the motors with the adjusted speeds
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, motorSpeedLeft);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, motorSpeedRight);

  delay(50);












//if (frontDistance < 10 && frontDistance > 3) {
  //  stopRobot(); // Stop the robot
    //moveback();
  //} else {
     
 // Check if any sensor detects a distance greater than 20 cm
  if (frontDistance > 20 && leftDistance <= 20 && rightDistance <= 20) {
    // Only the front sensor detects > 20 cm, move forward
    moveForward();
  } else if (frontDistance <= 15 && leftDistance > 20 && rightDistance <= 20) {
    // Only the left sensor detects > 20 cm, turn left and move forward
    turnLeft2();
    moveForward();
  } else if (frontDistance <= 15 && leftDistance <= 20 && rightDistance > 20) {
    // Only the right sensor detects > 20 cm, turn right and move forward
    turnRight2();
    moveForward();
  } else if (frontDistance > 20 && leftDistance > 20 && rightDistance <= 20) {
    // Front and left sensors detect > 20 cm, randomly choose between forward and left
    if (random(2) == 0) {
      moveForward();
    } else {
      turnLeft();
      moveForward();
    }
  } else if (frontDistance > 20 && leftDistance <= 20 && rightDistance > 20) {
    // Front and right sensors detect > 20 cm, randomly choose between forward and right
    if (random(2) == 0) {
      moveForward();
    } else {
      turnRight();
      moveForward();
    }
  } else if (frontDistance <= 15 && leftDistance > 20 && rightDistance > 20) {
    // Left and right sensors detect > 20 cm, randomly choose between left and right
    if (random(2) == 0) {
      turnLeft2();
      moveForward();
    } else {
      turnRight2();
      moveForward();
    }
  } else if (frontDistance > 20 && leftDistance > 20 && rightDistance > 20) {
    // All sensors detect > 20 cm, randomly choose between all three directions
    int direction = random(3);
    if (direction == 0) {
      moveForward();
    } else if (direction == 1) {
      turnLeft();
      moveForward();
    } else {
      turnRight();
      moveForward();
    }
  } else {//if (frontDistance < 10 && leftDistance < 10 && rightDistance < 10 &&frontDistance > 0 && leftDistance > 0 && rightDistance > 0) {
    // All sensors detect <= 20 cm, perform a turn-around
    moveBackward();
    turnAround();
  }
     
  delay(500); // Delay to control the loop execution speed

}

/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
float calculateDistance(NewPing& ultrasonic)
{
  // Calculate distance in centimeters
  return ultrasonic.ping_cm();
}

void calculateDistances(float& frontDistance, float& leftDistance, float& rightDistance)
{
  // Calculate distances from the ultrasonic sensors
  frontDistance = calculateDistance(frontUltrasonic);
  leftDistance = calculateDistance(leftUltrasonic);
  rightDistance = calculateDistance(rightUltrasonic);
}

void leftEncoderISR()
{
  
  leftEncoderCount++;
}

void rightEncoderISR()
{
  
  rightEncoderCount++;
}




void turnRight()
{
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  
  
  delay(500); 

  
  digitalWrite(in2, LOW);
  digitalWrite(in1, LOW);
  analogWrite(enA, 255);
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, 255);

  
  delay(405); // Adjust delay as needed
  
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(500);
}
void turnRight2()
{
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  
  
  delay(500); 

  
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(enA, 255);
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, 255);

  
  delay(200); // Adjust delay as needed
  
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(500);
}
void turnLeft()
{
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  
  
  delay(500); 

  
  digitalWrite(in2, LOW);
  digitalWrite(in1, HIGH);
  analogWrite(enA, 255);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  analogWrite(enB, 255);

  delay(405); 
  
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(500);
}
void turnLeft2()
{
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  
  
  delay(500); 

  
  digitalWrite(in2, LOW);
  digitalWrite(in1, HIGH);
  analogWrite(enA, 255);
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, 255);

  delay(200); 
  
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(500);
}


void turnAround()
{
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  
  
  delay(500); 

  
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(enA, 255);
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, 255);

  
  delay(380); 
  

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(500);
}

void moveForward()
{

  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 135);
  digitalWrite(in4, LOW);
  digitalWrite(in3, HIGH);
  analogWrite(enB, 135);
  delay(400); 
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);

}
void moveBackward()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 150);
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, 150);
  delay(150); // Adjust the delay for the desired backward movement time
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);
}
void moveback()
{digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 150);
  digitalWrite(in4, HIGH);
  digitalWrite(in3, LOW);
  analogWrite(enB, 150);
  delay(100); // Adjust the delay for the desired backward movement time
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(50);
}
void stopRobot()
{

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in4, LOW);
  digitalWrite(in3, LOW);
  delay(200);
}
void printGyroValues() {
  int16_t gyroX, gyroY, gyroZ;
  mpu.getRotation(&gyroX, &gyroY, &gyroZ);

  Serial.print("Gyro X: ");
  Serial.print(gyroX);
  Serial.print(" Gyro Y: ");
  Serial.print(gyroY);
  Serial.print(" Gyro Z: ");
  Serial.println(gyroZ);
}




