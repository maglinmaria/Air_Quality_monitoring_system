#define BLYNK_TEMPLATE_ID "TMPL3g6Fc1kES"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "JS5tPxl8TdCyiHHgbBn4hiqFiTNJl340"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "BKS";
char pass[] = "042006ka";

#define DHTPIN 2  // D4 on NodeMCU
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int gasPin = A0;

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD config

byte degree_symbol[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(gasPin);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send to Blynk
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, gasValue);

  // Send to Serial for Python voice
  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" C, Humidity: "); Serial.print(h);
  Serial.print(" %, Gas Value: "); Serial.println(gasValue);

  // Voice Alerts
  if (gasValue > 600) {
    Serial.println("VoiceAlert: Warning! Poor air quality detected.");
  } else if (gasValue > 400) {
    Serial.println("VoiceAlert: Air quality moderate.");
  } else {
    Serial.println("VoiceAlert: Air quality is good.");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, degree_symbol);

  lcd.setCursor(3,0);
  lcd.print("Air Quality");
  lcd.setCursor(3,1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();

  timer.setInterval(30000L, sendSensor); // Call every 30 sec
}

void loop() {
  Blynk.run();
  timer.run();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(gasPin);

  // LCD - Temperature
  lcd.setCursor(0,0);
  lcd.print("Temperature ");
  lcd.setCursor(0,1);
  lcd.print(t);
  lcd.setCursor(6,1);
  lcd.write(1);
  lcd.setCursor(7,1);
  lcd.print("C");
  delay(4000);
  lcd.clear();

  // LCD - Humidity
  lcd.setCursor(0,0);
  lcd.print("Humidity ");
  lcd.print(h);
  lcd.print("%");
  delay(4000);
  lcd.clear();

  // LCD - Gas
  lcd.setCursor(0,0);
  lcd.print("Gas Value: ");
  lcd.print(gasValue);
  lcd.setCursor(0,1);

  if (gasValue < 600) {
    lcd.print("Fresh Air");
    Serial.println("Fresh Air");
  } else {
    lcd.print("Bad Air");
    Serial.println("Bad Air");
    Blynk.logEvent("pollution_alert", "Bad Air");
  }

  delay(4000);
  lcd.clear();
}
