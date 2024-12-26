#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

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

  lcd.init();
  lcd.backlight();

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
  lcd.clear();

  if (is16BitMode) {
    lcd.setCursor(0, 0);
    lcd.print("16-bit mode:");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("8-bit mode:");
  }

  lcd.setCursor(0, 1);
  int maxBits = is16BitMode ? 16 : 8;
  for (int i = 0; i < maxBits; i++) {
    if (i == currentBit) {
      lcd.print("[");
    }
    lcd.print(bits[i]);
    if (i == currentBit) {
      lcd.print("]");
    }
    lcd.print(" ");
  }
}

void processEnteredData(int maxBits) {
  unsigned long value = 0;

  
  for (int i = 0; i < maxBits; i++) {
    value |= (bits[i] << (maxBits - 1 - i));
  }

  
  int pwmValue = map(value, 0, (1 << maxBits) - 1, 0, 255);
  analogWrite(pwmOutputPin, pwmValue);

  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PWM Updated");
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
