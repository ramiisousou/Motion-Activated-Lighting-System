#include <Servo.h>

int trigPin = 3;
int echoPin = 2;
int servoPin = 9;

int distanceThreshold = 30; // in cm
unsigned long cooldownTime = 2000; // 2 second "stall" to ensure no continous inputs
long autoOffTime = 5000;   // auto-off after 5 seconds

Servo switchServo;

bool lightIsOn = false;
bool objectPreviouslyDetected = false;
unsigned long lastToggleTime = 0;
unsigned long lightOnStartTime = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  switchServo.attach(servoPin);
  switchServo.write(0);  // Start OFF
  Serial.begin(9600);
}

void loop() {
  long duration, distance;

  // Send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read input
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  // Motion detected
  if (distance > 0 && distance < distanceThreshold) {
    if (!objectPreviouslyDetected && millis() - lastToggleTime > cooldownTime) {
      toggleSwitch();
      lastToggleTime = millis();
      objectPreviouslyDetected = true;

      // If just turned ON, record time
      if (lightIsOn) {
        lightOnStartTime = millis();
      }
    }
  } else {
    objectPreviouslyDetected = false;
  }

  // Auto turn OFF after timeout
  if (lightIsOn && (millis() - lightOnStartTime >= autoOffTime)) {
    Serial.println("Auto turn OFF after 5 seconds.");
    switchServo.write(0);
    lightIsOn = false;
  }

  delay(100);
}

void toggleSwitch() {
  if (!lightIsOn) {
    Serial.println("Switching ON.");
    switchServo.write(90);  // ON position
    lightIsOn = true;
    lightOnStartTime = millis(); // Start timer
  } else {
    Serial.println("Switching OFF.");
    switchServo.write(0);   // OFF position
    lightIsOn = false;
  }
}
