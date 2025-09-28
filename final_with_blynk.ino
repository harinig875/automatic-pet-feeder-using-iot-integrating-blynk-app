#include <WiFi.h>

#define BLYNK_TEMPLATE_ID "TMPL3J6uo4LKs"
#define BLYNK_TEMPLATE_NAME "Pet Feeder"

#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

// Define pins for the ultrasonic sensor and servo
const int trigPin = 5;
const int echoPin = 18;
const int servoPin = 19;

// Define bottle dimensions (in cm)
const float bottleHeight = 17.0; // Total height of the bottle
const float bottleRadius = 3.5;  // Radius of the cylindrical bottle

// Create Servo instance
Servo myServo;

// Blynk auth token
char auth[] = "dK_GqE_dmcTK3o_gaqcDwTTfScD5VyUk";
char ssid[] = "ENARXI";
char pass[] = "Enarxi12345@";

// Variables for Blynk control
bool feederEnabled = false;

// Timer for periodic tasks
BlynkTimer timer;

// Store timer handles
BlynkTimer::Handle startTimerHandle;
BlynkTimer::Handle stopTimerHandle;

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(servoPin); // Attach servo to the defined pin

  // Set a timer to call the measureFoodLevel function every second
  timer.setInterval(1000L, measureFoodLevel);
}

void loop() {
  Blynk.run();
  timer.run();
}

// Function to measure food level and send to Blynk
void measureFoodLevel() {
  // Trigger pulse to start measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo duration and calculate distance
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2.0;  // Calculate distance in centimeters

  // Calculate the height of the food column
  float foodHeight = bottleHeight - distance;
  foodHeight = max(foodHeight, 0.0f);

  // Calculate the volume of food (for a cylindrical bottle)
  float foodVolume = 3.14159 * bottleRadius * bottleRadius * foodHeight;

  // Print the distance, food height, and food volume to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Food Height: ");
  Serial.print(foodHeight);
  Serial.println(" cm");

  Serial.print("Food Volume: ");
  Serial.print(foodVolume);
  Serial.println(" cubic cm");

  // Control the servo based on the feederEnabled flag
  if (feederEnabled && foodHeight < 5.0) {
    dispenseFood();
  }
}

// Function to dispense food
void dispenseFood() {
  myServo.write(90); // Rotate servo to 90 degrees to dispense food
  delay(1000); // Wait for 1 second
  myServo.write(0); // Rotate servo back to 0 degrees
}

// Blynk function to control feeder on/off
BLYNK_WRITE(V1) {
  feederEnabled = param.asInt();
}
