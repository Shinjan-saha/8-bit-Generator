#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonTogglePin = 10; 
const int buttonNextPin = 11;   
const int pwmOutputPin = 9;     

bool is16BitMode = false;
int currentBit = 0;
int bits[16] = {0};       
bool buttonToggleState = false;
bool lastButtonToggleState = false;
bool buttonNextState = false;
bool lastButtonNextState = false;

void setup() {
  pinMode(buttonTogglePin, INPUT_PULLUP);
  pinMode(buttonNextPin, INPUT_PULLUP);
  pinMode(pwmOutputPin, OUTPUT);

  if (!display.begin(SSD1306_I2C_ADDRESS, SCREEN_ADDRESS)) {
    for (;;);
  }
  display.clearDisplay();
  display.display();

  resetBits();
}

void loop() {
  buttonToggleState = digitalRead(buttonTogglePin);
  buttonNextState = digitalRead(buttonNextPin);

  // Toggle mode between 8-bit and 16-bit when both buttons are pressed
  if (buttonToggleState == LOW && buttonNextState == LOW) {
    is16BitMode = !is16BitMode; 
    resetBits();
    delay(500); 
  }

  if (buttonToggleState == LOW && lastButtonToggleState == HIGH) {
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }

  if (buttonNextState == LOW && lastButtonNextState == HIGH) {
    int maxBits = is16BitMode ? 16 : 8;

    if (currentBit == maxBits - 1) {
      processEnteredData(maxBits); 
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

  if (is16BitMode) {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("16-bit mode:");
  } else {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("8-bit mode:");
  }

  display.setCursor(0, 16);
  int maxBits = is16BitMode ? 16 : 8;
  for (int i = 0; i < maxBits; i++) {
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

void processEnteredData(int maxBits) {
  unsigned long value = 0;

  for (int i = 0; i < maxBits; i++) {
    value |= (bits[i] << (maxBits - 1 - i));
  }

  int pwmValue = map(value, 0, (1 << maxBits) - 1, 0, 255);
  analogWrite(pwmOutputPin, pwmValue);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("PWM Updated");
  display.display();
  delay(1000); 

  resetBits();
}

void resetBits() {
  for (int i = 0; i < 16; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}
