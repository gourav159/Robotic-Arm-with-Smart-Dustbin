#include <Servo.h>

// Smart Dustbin - Ultrasonic Sensor Pins
#define TRIG_PIN 2
#define ECHO_PIN 3

// Smart Dustbin - Lid Servo
Servo servoLid;

// Robotic Arm Servos
Servo baseServo, shoulderServo, elbowServo, gripperServo;

// Robotic Arm Servo Positions
int basePos = 110, shoulderPos = 40, elbowPos = 160, gripperPos = 150;
int stepSize = 20;

void setup() {
    Serial.begin(9600);

    // Smart Dustbin Setup
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    servoLid.attach(5); // Lid servo on pin 5
    servoLid.write(0);  // Keep lid closed initially

    // Robotic Arm Setup
    baseServo.attach(6);      // Base Servo
    shoulderServo.attach(9);  // Shoulder Servo
    elbowServo.attach(10);    // Elbow Servo
    gripperServo.attach(11);  // Gripper Servo

    baseServo.write(basePos);
    shoulderServo.write(shoulderPos);
    elbowServo.write(elbowPos);
    gripperServo.write(gripperPos);
}

long getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    long distance = duration * 0.034 / 2;
    return distance;
}

void loop() {
    // --- Smart Dustbin Logic ---
    long distance = getDistance();
    if (distance < 10) {
        servoLid.write(90); // Open lid
        
        // --- Robotic Arm Bluetooth Control ---
        // Only works when distance < 10
        if (Serial.available()) {
            String command = Serial.readStringUntil('\n');
            command.trim();

            if (command == "BR" && basePos > 0) basePos -= stepSize;
            if (command == "BL" && basePos < 180) basePos += stepSize;
            if (command == "SD" && shoulderPos < 180) shoulderPos += stepSize;
            if (command == "SU" && shoulderPos > 0) shoulderPos -= stepSize;
            if (command == "EU" && elbowPos < 180) elbowPos += stepSize;
            if (command == "ED" && elbowPos > 0) elbowPos -= stepSize;
            if (command == "GC" && gripperPos < 150) gripperPos += stepSize;
            if (command == "GO" && gripperPos > 100) gripperPos -= stepSize;

            baseServo.write(basePos);
            shoulderServo.write(shoulderPos);
            elbowServo.write(elbowPos);
            gripperServo.write(gripperPos);

            Serial.print("Base: "); Serial.print(basePos);
            Serial.print(" | Shoulder: "); Serial.print(shoulderPos);
            Serial.print(" | Elbow: "); Serial.print(elbowPos);
            Serial.print(" | Gripper: "); Serial.println(gripperPos);
        }
    } else {
        servoLid.write(0); // Close lid
    }

    delay(100); // Small delay to avoid overload
}