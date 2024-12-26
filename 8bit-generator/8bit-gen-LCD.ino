#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PWM_OUTPUT_PIN 9         
#define BUTTON_TOGGLE_PIN 10    
#define BUTTON_NEXT_PIN 11       


LiquidCrystal_I2C lcd(0x27, 16, 2);


int currentBit = 0;
int bits[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
bool buttonToggleState = false;
bool lastButtonToggleState = false;
bool buttonNextState = false;
bool lastButtonNextState = false;

void setup() {
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter 8-bit:");

  
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter 8-bit:");

  lcd.setCursor(0, 1);
  for (int i = 0; i < 8; i++) {
    if (i == currentBit) {
      lcd.print("[");
    }
    lcd.print(bits[i]);
    if (i == currentBit) {
      lcd.print("]");
    } else {
      lcd.print(" ");
    }
  }
}


void updatePWMOutput() {
 
  int pwmValue = 0;
  for (int i = 0; i < 8; i++) {
    pwmValue |= (bits[i] << (7 - i)); 
  }

  
  analogWrite(PWM_OUTPUT_PIN, pwmValue);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PWM Updated");
  delay(1000); 
}


void resetBits() {
  for (int i = 0; i < 8; i++) {
    bits[i] = 0; 
  }
  currentBit = 0;
  updateDisplay();
}
