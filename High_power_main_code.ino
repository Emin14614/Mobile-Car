#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX, TX

// Pin assignments
int R_led = 12; 
int L_led = 4; 
int Reverse_led = 7; 
int Flash_led = 8; 
int FR_motor_p = 6; 
int FR_motor_n = 5; 
int FL_motor_p = 9; 
int FL_motor_n = 10;
int BUZZER_PIN = 13; // New pin for the buzzer

// Ultrasonic Sensor Pins
int FRONT_TRIG_PIN = A0;
int FRONT_ECHO_PIN = A1;
int BACK_TRIG_PIN = A2;
int BACK_ECHO_PIN = A3;

// Set a threshold distance in centimeters
int OBSTACLE_DISTANCE_CM = 35;

char BTRead = 'A';
char Command = 'S'; // Default to 'S' (Stop) for safety

// Variables for non-blocking buzzer control
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

// Global variables for the LED timer
unsigned long ledStartTime = 0;
const long ledDuration = 2500; // 2.5 seconds in milliseconds
bool ledsOn = false;

// Function to get distance from a sensor
long getDistance(int trigPin, int echoPin) {
  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Send a high pulse to trigger sensor and Avinash
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Measure the duration of the echo pulse with a timeout
  long duration = pulseIn(echoPin, HIGH, 20000); 
  if (duration == 0) {
    return 300; // Return a safe large distance if no echo
  }
  return duration * 0.034 / 2; // Converts microseconds to centimeters
}

void setup() 
{
  pinMode(R_led, OUTPUT);
  pinMode(L_led, OUTPUT);
  pinMode(Reverse_led, OUTPUT);
  pinMode(Flash_led, OUTPUT);
  pinMode(FR_motor_p, OUTPUT);
  pinMode(FR_motor_n, OUTPUT);
  pinMode(FL_motor_p, OUTPUT);
  pinMode(FL_motor_n, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); // Configure the buzzer pin

  // Set up sensor pins
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(BACK_TRIG_PIN, OUTPUT);
  pinMode(BACK_ECHO_PIN, INPUT);

  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("HC-05 with Arduino - Ready");
}

void loop() 
{
  // Read sensor distances for a quick reaction
  long front_distance = getDistance(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
  long back_distance = getDistance(BACK_TRIG_PIN, BACK_ECHO_PIN);

  // Check for obstacles and force a stop
  if (((Command == 'F' && front_distance <= OBSTACLE_DISTANCE_CM) || (Command == 'B' && back_distance <= OBSTACLE_DISTANCE_CM))) {
    // Check if the command is NOT already 'S' to avoid re-triggering
    if (Command != 'S') 
    {
      buzzerActive = true;
      buzzerStartTime = millis();
      // Start the LED timer
      ledsOn = true;
      ledStartTime = millis();
      digitalWrite(L_led, HIGH);
      digitalWrite(R_led, HIGH);
    }
    Command = 'S';

    Serial.println("Obstacle Detected! Forcing Stop.");
    // The following lines are removed because they conflict with the timer logic:
    // digitalWrite(L_led, HIGH);
    // digitalWrite(R_led, HIGH);
  }
  
  // Read new command from Bluetooth
  if (BTSerial.available() > 0) {
    BTRead = BTSerial.read();
    if ((BTRead == 'F') || (BTRead == 'B') || (BTRead == 'L') || (BTRead == 'R') || (BTRead == 'S')) {
      Command = BTRead;
      Serial.print("New Command: ");
      Serial.println(Command);
    }
  }

  // Non-blocking buzzer control
  if (buzzerActive) {
    if (millis() - buzzerStartTime < 2500) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false; // Stop the buzzer after 2.5 seconds
    }
  }

  // Non-blocking LED timer
  if (ledsOn && (millis() - ledStartTime >= ledDuration)) {
    digitalWrite(L_led, LOW); // Turn LEDs OFF
    digitalWrite(R_led, LOW);
    ledsOn = false; // Reset the flag
  }

  // Update motor and LED states based on the current command
  switch (Command) {
    case 'F': // Move Forward
      digitalWrite(R_led, LOW);
      digitalWrite(L_led, LOW);
      digitalWrite(Reverse_led, LOW);
      digitalWrite(Flash_led, HIGH);
      analogWrite(FR_motor_p, 255);
      analogWrite(FR_motor_n, 0);
      analogWrite(FL_motor_p, 255);
      analogWrite(FL_motor_n, 0);
      break;

    case 'B': // Move Backward
      digitalWrite(R_led, LOW);
      digitalWrite(L_led, LOW);
      digitalWrite(Reverse_led, HIGH);
      digitalWrite(Flash_led, LOW);
      analogWrite(FR_motor_p, 0);
      analogWrite(FR_motor_n, 255);
      analogWrite(FL_motor_p, 0);
      analogWrite(FL_motor_n, 255);
      break;

    case 'L': // Turn Left
      digitalWrite(R_led, LOW);
      digitalWrite(L_led, HIGH);
      digitalWrite(Reverse_led, LOW);
      digitalWrite(Flash_led, HIGH);
      analogWrite(FR_motor_p, 250);
      analogWrite(FR_motor_n, 0);
      analogWrite(FL_motor_p, 0);
      analogWrite(FL_motor_n, 250);
      break;

    case 'R': // Turn Right
      digitalWrite(R_led, HIGH);
      digitalWrite(L_led, LOW);
      digitalWrite(Reverse_led, LOW);
      digitalWrite(Flash_led, HIGH);
      analogWrite(FR_motor_p, 0);
      analogWrite(FR_motor_n, 250);
      analogWrite(FL_motor_p, 250);
      analogWrite(FL_motor_n, 0);
      break;

    case 'S':
      digitalWrite(Reverse_led, HIGH);
      digitalWrite(Flash_led, LOW);
      digitalWrite(R_led, LOW);
      digitalWrite(L_led, LOW);
      // Fixed by removing digitalWrite(BUZZER_PIN, LOW); from here
      analogWrite(FR_motor_p, 0);
      analogWrite(FR_motor_n, 0);
      analogWrite(FL_motor_p, 0);
      analogWrite(FL_motor_n, 0);
      break;

    default: 
      digitalWrite(R_led, LOW);
      digitalWrite(L_led, LOW);
      digitalWrite(Reverse_led, LOW);
      digitalWrite(Flash_led, LOW);
      analogWrite(FR_motor_p, 0);
      analogWrite(FR_motor_n, 0);
      analogWrite(FL_motor_p, 0);
      analogWrite(FL_motor_n, 0);
      break; 
  }
}