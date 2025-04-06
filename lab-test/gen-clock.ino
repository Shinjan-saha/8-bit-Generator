#include <Wire.h>

const int buttonTogglePin = 10; 
const int buttonNextPin = 11;   
const int pwmOutputPin = 9;     
const int clockOutputPin = 6;   // PWM pin for 490Hz clock

bool is16BitMode = false;
int currentBit = 0;
int bits[16] = {0};       
bool buttonToggleState = false;
bool lastButtonToggleState = false;
bool buttonNextState = false;
bool lastButtonNextState = false;

// New serial command variables
String inputString = "";
bool stringComplete = false;

void setup() {
  pinMode(buttonTogglePin, INPUT_PULLUP);
  pinMode(buttonNextPin, INPUT_PULLUP);
  pinMode(pwmOutputPin, OUTPUT);
  pinMode(clockOutputPin, OUTPUT);

  // Initialize serial communication
  Serial.begin(9600);
  
  // Print welcome message and instructions
  printInstructions();

  resetBits();
  startClock();  // Start the 490Hz clock signal
  
  // Reserve 200 bytes for the inputString
  inputString.reserve(200);
}

void loop() {
  // Check physical buttons (keeping this functionality)
  buttonToggleState = digitalRead(buttonTogglePin);
  buttonNextState = digitalRead(buttonNextPin);

  // Toggle mode between 8-bit and 16-bit when both buttons are pressed
  if (buttonToggleState == LOW && buttonNextState == LOW) {
    is16BitMode = !is16BitMode; 
    resetBits();
    delay(500);
    updateDisplay();
  }

  if (buttonToggleState == LOW && lastButtonToggleState == HIGH) {
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }

  if (buttonNextState == LOW && lastButtonNextState == HIGH) {
    int maxBits = is16BitMode ? 16 : 8;

    if (currentBit == maxBits - 1) {
       startTransmission(maxBits);
    } else {
      currentBit++;
      updateDisplay();
    }
  }

  lastButtonToggleState = buttonToggleState;
  lastButtonNextState = buttonNextState;

  // Process any serial commands
  if (stringComplete) {
    processSerialCommand();
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void processSerialCommand() {
  inputString.trim();
  
  if (inputString.equalsIgnoreCase("mode")) {
    is16BitMode = !is16BitMode;
    resetBits();
    Serial.print("Switched to ");
    Serial.print(is16BitMode ? "16" : "8");
    Serial.println("-bit mode");
    updateDisplay();
  } 
  else if (inputString.equalsIgnoreCase("toggle")) {
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }
  else if (inputString.equalsIgnoreCase("next")) {
    int maxBits = is16BitMode ? 16 : 8;
    if (currentBit == maxBits - 1) {
      startTransmission(maxBits);
    } else {
      currentBit++;
      updateDisplay();
    }
  }
  else if (inputString.equalsIgnoreCase("reset")) {
    resetBits();
    updateDisplay();
  }
  else if (inputString.equalsIgnoreCase("start")) {
    int maxBits = is16BitMode ? 16 : 8;
    startTransmission(maxBits);
  }
  else if (inputString.equalsIgnoreCase("help")) {
    printInstructions();
  }
  else if (inputString.startsWith("set ")) {
    // Set all bits at once with format "set 10101010" or "set 1010101010101010"
    String bitString = inputString.substring(4);
    int maxBits = is16BitMode ? 16 : 8;
    
    if (bitString.length() == maxBits) {
      for (int i = 0; i < maxBits; i++) {
        if (bitString.charAt(i) == '1') {
          bits[i] = 1;
        } else if (bitString.charAt(i) == '0') {
          bits[i] = 0;
        }
      }
      Serial.println("Bits set successfully");
      updateDisplay();
    } else {
      Serial.print("Error: Please provide exactly ");
      Serial.print(maxBits);
      Serial.println(" bits");
    }
  }
  else {
    Serial.println("Unknown command. Type 'help' for instructions.");
  }
}

void updateDisplay() {
  int maxBits = is16BitMode ? 16 : 8;
  
  Serial.print(is16BitMode ? "16" : "8");
  Serial.println("-bit mode:");
  
  for (int i = 0; i < maxBits; i++) {
    if (i == currentBit) {
      Serial.print("[");
    }
    Serial.print(bits[i]);
    if (i == currentBit) {
      Serial.print("]");
    }
    Serial.print(" ");
  }
  Serial.println();
}

void printInstructions() {
  Serial.println("\n========= Bit Pattern Generator =========");
  Serial.println("Available commands:");
  Serial.println("  mode   - Toggle between 8-bit and 16-bit mode");
  Serial.println("  toggle - Toggle current bit value (0/1)");
  Serial.println("  next   - Move to next bit position");
  Serial.println("  reset  - Reset all bits to 0");
  Serial.println("  start  - Start continuous data transmission");
  Serial.println("  set XXXXXXXX - Set all bits at once (use 0s and 1s)");
  Serial.println("  help   - Show these instructions");
  Serial.println("========================================\n");
}

void startTransmission(int maxBits) {
  Serial.println("Streaming data...");
  Serial.println("Send any character to stop streaming");

  while (!Serial.available()) { // Stream until serial input is received
    for (int i = 0; i < maxBits; i++) {
      digitalWrite(pwmOutputPin, bits[i]);  // Output bit (1 or 0)
      delay(100); // Adjust for timing control
    }

    // Check if user wants to reset with physical buttons
    if (digitalRead(buttonTogglePin) == LOW && digitalRead(buttonNextPin) == LOW) {
      Serial.println("Stopped streaming (button interrupt)");
      digitalWrite(pwmOutputPin, LOW);  // Ensure the output is LOW when stopping
      resetBits();
      return; // Exit the loop and return to normal operation
    }
  }
  
  // Clear any received characters
  while (Serial.available()) {
    Serial.read();
  }
  
  Serial.println("Stopped streaming");
  digitalWrite(pwmOutputPin, LOW);  // Ensure the output is LOW when stopping
}

void resetBits() {
  for (int i = 0; i < 16; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
}

// Function to generate a 490Hz clock signal
void startClock() {
  // For 490Hz on Arduino Uno, we can use direct port manipulation or tone()
  // Using analogWrite for simplicity
  analogWrite(clockOutputPin, 128); // 50% duty cycle for a square wave
}