// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 7, d5 = 12, d6 = 11, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN A1     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

int PWM_INPUT;
#define PWMIN A0
#define PWMPIN 3

#define TACHPIN 2
unsigned int RPM;
volatile byte PULSES;
unsigned long TIMEOLD;
unsigned int PULSESRT = 1;

void COUNTER() {
  PULSES++;
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("hello, world!");

  Serial.begin(9600);

  // Initialize temp/humidity sensor
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  // Initialize PWM control
  pinMode(PWMIN, INPUT);
  pinMode (PWMPIN, OUTPUT);
  
  // Initialize RPM read
  pinMode (TACHPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TACHPIN), COUNTER, FALLING);
  PULSES = 0;
  RPM = 0;
  TIMEOLD = 0;
  
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    lcd.setCursor(11, 0);
    lcd.print("T:");
    lcd.print(event.temperature,0);
//   lcd.setCursor(, 1);
    lcd.print ((char) 223);
//    lcd.print ("C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    lcd.setCursor(11, 1);
    lcd.print("H:");
    lcd.print(event.relative_humidity,0);
//    lcd.setCursor(15, 1);
    lcd.print ("%");
  }

  unsigned long A=analogRead(PWMIN);
  Serial.print("PWNIN = ");
  Serial.println(A);
  unsigned long DUTY = A * 100 / 1023;
  Serial.print("DUTY = ");
  Serial.print (DUTY);
  Serial.println ("%");
  lcd.setCursor(0, 1);
  lcd.print ("DUTY:");
  lcd.print (DUTY);
  lcd.print ("% ");

  unsigned long PWMOUT = A/4;
  analogWrite(PWMPIN, A/4);
  Serial.print ("PWMOUT = ");
  Serial.println (PWMOUT);

  if (millis() - TIMEOLD >= 2000) {
    detachInterrupt(digitalPinToInterrupt(TACHPIN));
    RPM = (30 * 2000 / PULSESRT /2  ) / (millis() - TIMEOLD) * PULSES;
    TIMEOLD = millis ();
    PULSES = 0;
  }
  Serial.print ("RPM = ");
  Serial.println (RPM);
  lcd.setCursor(0,0);
  lcd.print ("RPM :");
  lcd.print (RPM);
  if ( RPM >= 1000) {  lcd.print (" "); }
  else lcd.print ("  ");
  
  attachInterrupt(digitalPinToInterrupt(TACHPIN), COUNTER, FALLING);
//  delay (1000);
}
