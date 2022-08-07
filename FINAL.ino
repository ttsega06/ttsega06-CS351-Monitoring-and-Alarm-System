

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"
#include <IRremote.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int hours = 0;
int minutes = 0;
int seconds = 0;
long lastSecond = 0;


#define PIN_DHT 10
DHT dht(PIN_DHT, DHT11);
float humidity;
float temperature;
bool accident = false;

#define PIN_BUZZER 9

#define PIN_IR 8

void setup() {

  Serial.begin(9600);

  pinMode(PIN_DHT, INPUT);
  digitalWrite(PIN_DHT, HIGH);

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_IR, INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print("T=");
  lcd.setCursor(8, 1);
  lcd.print("H=");

  dht.begin();
  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
}

void loop() {

  //update time every 1 second and take new temp/hum sample
  if (millis() - lastSecond >= 1000) {
    //only update the time if humidity is under 85
    
    lastSecond = millis();
    seconds++;
    if (accident == false) {
      updateTime();
    }
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    lcd.setCursor(2, 1);
    lcd.print(temperature, 1);
    lcd.setCursor(10, 1);
    lcd.print(humidity, 1);
  }
  //I couldn't get the humidity to get to 85 by holding it so I had to lower this
  //number to do the demo
  if (humidity >= 50) {
    accident = true;
    tone(PIN_BUZZER, 500);
  }

  if (IrReceiver.decode() && accident == false) {
    IrReceiver.resume();
    switch (IrReceiver.decodedIRData.command) {
      case 0xC:
        //Serial.println("1");
        hours++;        
        break;
      case 0x18:
        //Serial.println("2");
        minutes++;
        break;
      case 0x5E:
        seconds = 0;
        //Serial.println("3");        
        break;
      case 0x8:
        hours--;
        //Serial.println("4");
        break;
      case 0x1C:
        minutes--;
        //Serial.println("5");
        break;
    }
    updateTime();
    //stop double presses
    delay(250);
  }
}

void updateTime() {
  
  if (seconds == 60) {
    minutes++;
    seconds = 0;
  }//the remote makes the time go negative 
  else if (seconds < 0){
    minutes--;
    seconds = 59;
  }

