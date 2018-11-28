#include "dht11.h"
#include "Adafruit_SleepyDog.h"
#include "LiquidCrystal_I2C.h"
#include "DS3231.h"


LiquidCrystal_I2C lcd(0x27,20,4); // SDA -> A4 SCL -> A5
RTClib RTC;
dht11 DHT11;

static boolean blink = true;

#define SM_PIN 0 // A0
#define DHT_PIN 2 // D2
#define RELAY_PIN_1 3
#define RELAY_PIN_2 4
#define RELAY_PIN_3 5
#define RELAY_PIN_4 6
#define LIGHT_1 7


#define SOIL_MAX 600
#define SOIL_MIN 300

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LIGHT_1, OUTPUT);
  

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Initializing system.");
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
  lcd.print("Reset at ");
  lcd.print(countDown/1000); lcd.print(" seconds");
  delay(2000);

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Temperature: ");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.setCursor(0,2);
  lcd.print("Soil Moisture: ");

  // digitalWrite(LIGHT_1, HIGH);
  
}

void loop() {
  
  DHT11.read(DHT_PIN);// initialize the reading
  int humidity = DHT11.humidity;// get humidity

  lcd.setCursor(13, 0);
  lcd.print((int)getTemp('C'));
  lcd.print("C");
  
  lcd.setCursor(10, 1);
  lcd.print(humidity);
  lcd.print("%");
  
  lcd.setCursor(15, 2);
  lcd.print((int)getSoilMoisture());
  lcd.print("%");

  DateTime now = RTC.now();
  lcd.setCursor(0,3);
  lcd.print(now.year(), DEC);
  

  /** 
   *  Water pump
   */
   /*
  if (certainTime) {
    digitalWrite(RELAY_PIN_1, HIGH);
    delay(1000); // Turn on for 1 second
    digitalWrite(RELAY_PIN_1, LOW);
  }
  */
  
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

float getSoilMoisture() {
  float reading = (float)analogRead(SM_PIN);
  float calc = ((SOIL_MAX - reading)/(SOIL_MAX - SOIL_MIN)) * 100;
  
  return calc;
}
