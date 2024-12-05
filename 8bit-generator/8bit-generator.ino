#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32


#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int bitPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};


const int buttonTogglePin = 10;
const int buttonNextPin = 11;

// Variables
int currentBit = 0;
int bits[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
bool buttonToggleState = false;     
bool lastButtonToggleState = false; 
bool buttonNextState = false;       
bool lastButtonNextState = false;   

void setup() {
  
  for (int i = 0; i < 8; i++) {
    pinMode(bitPins[i], OUTPUT);
  }

  
  pinMode(buttonTogglePin, INPUT_PULLUP);
  pinMode(buttonNextPin, INPUT_PULLUP);

  
  if (!display.begin(0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  updateDisplay();
}

void loop() {

  buttonToggleState = digitalRead(buttonTogglePin);
  buttonNextState = digitalRead(buttonNextPin);

  
  if (buttonToggleState == LOW && lastButtonToggleState == HIGH) {
    
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }

  
  if (buttonNextState == LOW && lastButtonNextState == HIGH) {
    if (currentBit == 7) {

      updateLEDs();
      delay(1000); 
      resetBits();
    } else {
      currentBit++; 
    }
    updateDisplay();
  }

  
  lastButtonToggleState = buttonToggleState;
  lastButtonNextState = buttonNextState;
}


void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);

  
  display.print("Enter 8-bit number:");
  display.setCursor(0, 10);

  
  for (int i = 0; i < 8; i++) {
    if (i == currentBit) {
      display.print("[");
    }
    display.print(bits[i]);
    if (i == currentBit) {
      display.print("]");
    }
    display.print(" ");
  }

  display.display();
}


void updateLEDs() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(bitPins[i], bits[i]);
  }
}


void resetBits() {
  for (int i = 0; i < 8; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}
