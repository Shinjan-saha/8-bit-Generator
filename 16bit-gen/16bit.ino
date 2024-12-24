#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonTogglePin = 10; 
const int buttonNextPin = 11;    

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
  display.setCursor(0, 0);

  if (is16BitMode) {
    display.print("Enter 16-bit number:");
  } else {
    display.print("Enter 8-bit number:");
  }
  display.setCursor(0, 10);


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


  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Entered Value:");
  display.setCursor(0, 10);
  display.print(value, DEC); 
  display.setCursor(0, 20);
  display.print("Hex: 0x");
  display.print(value, HEX); 
  display.display();
}

void resetBits() {
  for (int i = 0; i < 16; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}
