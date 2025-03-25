#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

const int buttonTogglePin = 10; 
const int buttonNextPin = 11;   
const int pwmOutputPin = 9;     // Data output pin (adjusted for 8kHz/20kHz PWM)
const int clockOutputPin = 6;   // Clock output (modified to 8kHz/20kHz)

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
  pinMode(clockOutputPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  resetBits();
  
  setPWMFrequency(9, 8000);   // Set Data output (Pin 9) to 8kHz
  setClockFrequency(6, 8000); // Set Clock output (Pin 6) to 8kHz
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
       startTransmission(maxBits);
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

void startTransmission(int maxBits) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Streaming data...");

  while (true) { 
    for (int i = 0; i < maxBits; i++) {
      analogWrite(pwmOutputPin, bits[i] ? 255 : 0);  
      delayMicroseconds(125);  // Adjust timing based on frequency
    }

    if (digitalRead(buttonTogglePin) == LOW && digitalRead(buttonNextPin) == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Stopped!");
      analogWrite(pwmOutputPin, 0);
      resetBits();
      return;
    }
  }
}

void resetBits() {
  for (int i = 0; i < 16; i++) {
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}

// Function to change PWM frequency on Pin 9 (Data)
void setPWMFrequency(int pin, int frequency) {
  if (pin == 9 || pin == 10) {
    // Configure Timer1 (affects pin 9 and pin 10)
    TCCR1A = _BV(COM1A1) | _BV(WGM11);  
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); 

    if (frequency == 8000) {  
      ICR1 = 2000;  // 8 kHz frequency
    } else if (frequency == 20000) {  
      ICR1 = 800;   // 20 kHz frequency
    }
  }
}

// Function to change Clock Frequency on Pin 6
void setClockFrequency(int pin, int frequency) {
  if (pin == 6) {
    // Configure Timer0 for Fast PWM Mode (affects pin 5 and pin 6)
    TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);  // Fast PWM mode
    TCCR0B = _BV(WGM02) | _BV(CS01); // Prescaler: /8 for higher frequencies

    if (frequency == 8000) {  
      OCR0A = 249;  // 8 kHz frequency
    } else if (frequency == 20000) {  
      OCR0A = 99;   // 20 kHz frequency
    }
  }
}
