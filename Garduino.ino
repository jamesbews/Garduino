#include "dht11.h"
#include "Adafruit_SleepyDog.h"
#include "LiquidCrystal_I2C.h"
#include "DS3232RTC.h"


LiquidCrystal_I2C lcd(0x27,20,4); // SDA -> A4 SCL -> A5
dht11 DHT11;

static boolean blink = true;
static boolean lightOneOn = false;
static boolean lightTwoOn = false;
static boolean motionDetected = false;

static int lcdTimer = 10; // Turn on lcd for 10 seconds

#define PLANT_1 0 // A0
#define PLANT_2 1 // A1
#define DHT_PIN 2 // D2
#define PIR_PIN 3 // D3
#define LIGHT_1 5 // D5
#define LIGHT_2 6 // D6
// D7 is broken


#define SOIL_MAX 600
#define SOIL_MIN 300

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_1, OUTPUT);
  pinMode(LIGHT_2, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Initializing...");
  delay(2000);
  while(!Serial) {
    delay(1000);
  }
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Testing system...");
  lcd.setCursor(0,2);
  int countDown = Watchdog.enable(4000); // 4 seconds
  
  for (int i = 0; i < 5; i++) {
    delay(2000);
    Watchdog.reset();
    lcd.print("* ");
  }

  lcd.setCursor(0,3);
  lcd.print("Tests passed ");
  delay(2000);

  lcd.clear();

  setTime(17, 47, 10, 6, 1, 2019); // hour, minute, second, day, month year
  RTC.set(now());
       
}

void loop() {

  detectMovement();

  if (lcdTimer > 0) {
    lcdTimer--;
  }

  if (lcdTimer > 0) {
    printTempAndHumidity();

    printSoilMoisture();
  
    digitalClockDisplay();
  }

  if (lcdTimer == 0) {
    lcd.noBacklight(); // Turn off lcd
  }

  if (!lightOneOn && hour() >= 6) {
    digitalWrite(LIGHT_1, HIGH);
    lightOneOn = true;
  }
  if (!lightTwoOn && hour() >= 7) {
    digitalWrite(LIGHT_2, HIGH);
    lightTwoOn = true;
  }

  if (lightOneOn && hour() >= 20) {
    digitalWrite(LIGHT_1, LOW);
    lightOneOn = false;
  }
  if (lightTwoOn && hour() >= 21) {
    digitalWrite(LIGHT_2, LOW);
    lightTwoOn = false;
  }

  
  if (blink){
    digitalWrite(LED_BUILTIN, HIGH);
    blink = !blink;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    blink = !blink;
  }
  delay(1000);
  Watchdog.reset();

}

float getTemp(char type) {
    float temp = (float)DHT11.temperature;//get temp
  if(type =='F')
  {
    return temp * 1.8 + 32;// convert to fahrenheit
  }else if(type =='K')
  {
    return temp + 274.15;// convert to Kelvin
  }else{
   return temp; 
  }
  
}

float getSoilMoisture(int plant) {
  float reading = (float)analogRead(plant);
  float calc = ((SOIL_MAX - reading)/(SOIL_MAX - SOIL_MIN)) * 100;
  
  return calc;
}

void printTempAndHumidity() {
  DHT11.read(DHT_PIN);// initialize the reading
  int humidity = DHT11.humidity;// get humidity

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print((int)getTemp('C'));
  lcd.print("C");
  lcd.print(" ");
  
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("% ");
}

void printSoilMoisture() {
  lcd.setCursor(0, 1);
  lcd.print("Plant 1: ");
  lcd.print((int)getSoilMoisture(PLANT_1));
  lcd.print("% ");

  lcd.setCursor(0, 2);
  lcd.print("Plant 2: ");
  lcd.print((int)getSoilMoisture(PLANT_2));
  lcd.print("% ");
}

void detectMovement() {
  int val = digitalRead(PIR_PIN);
  if (val == HIGH) {
    lcd.backlight(); // Turn on lcd backlight
    lcdTimer = 10;
  }
}

void digitalClockDisplay()
{
    lcd.setCursor(0,3);
    lcd.print(hour());
    printDigits(minute());
    printDigits(second());
    lcd.print(' ');
    lcd.print(' ');
    lcd.print(day());
    lcd.print('/');
    lcd.print(month());
    lcd.print('/');
    lcd.print(year());
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    lcd.print(':');
    if(digits < 10)
        lcd.print('0');
    lcd.print(digits);
}
