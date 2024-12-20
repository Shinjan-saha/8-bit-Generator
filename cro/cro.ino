#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int pwmInputPin = 14;
unsigned long highTime, lowTime;
float dutyCycle = 0;
float frequency = 0;

void setup() {
  pinMode(pwmInputPin, INPUT);
  Serial.begin(9600);

  if (!display.begin(0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

void loop() {
  
  highTime = pulseIn(pwmInputPin, HIGH);
  lowTime = pulseIn(pwmInputPin, LOW);

  if (highTime > 0 && lowTime > 0) {
    
    unsigned long period = highTime + lowTime; 
    frequency = 1000000.0 / period;
    dutyCycle = (float)highTime / period * 100; 

    
    displayData(dutyCycle, frequency);
  } else {

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No PWM Signal");
    display.display();
  }

  delay(500);
}

void displayData(float duty, float freq) {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("PWM Signal Data");
  display.setCursor(0, 10);
  display.print("Frequency: ");
  display.print(freq);
  display.print(" Hz");

  display.setCursor(0, 20);
  display.print("Duty Cycle: ");
  display.print(duty);
  display.print(" %");

  display.display();
}
