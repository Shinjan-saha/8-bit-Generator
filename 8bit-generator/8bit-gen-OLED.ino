#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PWM_OUTPUT_PIN 9         
#define BUTTON_TOGGLE_PIN 10    
#define BUTTON_NEXT_PIN 11       

int currentBit = 0;
int bits[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
bool buttonToggleState = false;
bool lastButtonToggleState = false;
bool buttonNextState = false;
bool lastButtonNextState = false;

void setup() {
 
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) { 
    for (;;);
  }
  display.clearDisplay();
  display.display();

  
  pinMode(PWM_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);

 
  updateDisplay();
}

void loop() {
  
  buttonToggleState = digitalRead(BUTTON_TOGGLE_PIN);
  buttonNextState = digitalRead(BUTTON_NEXT_PIN);

  
  if (buttonToggleState == LOW && lastButtonToggleState == HIGH) {
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }

  
  if (buttonNextState == LOW && lastButtonNextState == HIGH) {
    if (currentBit == 7) {
      updatePWMOutput(); 
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

 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Enter 8-bit:");

  
  display.setCursor(0, 20);
  for (int i = 0; i < 8; i++) {
    if (i == currentBit) {
      display.print("[");
    }
    display.print(bits[i]);
    if (i == currentBit) {
      display.print("]");
    } else {
      display.print(" ");
    }
  }

 
  display.display();
}

void updatePWMOutput() {
  
  int pwmValue = 0;
  for (int i = 0; i < 8; i++) {
    pwmValue |= (bits[i] << (7 - i));
  }

  
  analogWrite(PWM_OUTPUT_PIN, pwmValue);

 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("PWM Updated");
  display.display();
  delay(1000); 
}

void resetBits() {
 
  for (int i = 0; i < 8; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}
