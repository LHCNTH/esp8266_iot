
#define BLYNK_TEMPLATE_ID "TMPL6aXoA2tT-"
#define BLYNK_TEMPLATE_NAME "Do An 3"
#define BLYNK_AUTH_TOKEN "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define SDA 4  // GPIO4 (D2)
#define SCL 5  // GPIO5 (D1)
#define DHTPIN 2  // Chân DHT11 nối với GPIO2 (D4)
#define DHTTYPE DHT11
#define LED_PIN 0  // Chân GPIO0 (D3)
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledState = LOW;
bool userControll = false;
unsigned long lastControll = 0;  
// nếu trong chế độ true thì hệ thống sẽ tự động bật/tắt đèn
// còn chế độ false là người dùng bật/tắt đèn

char auth[] = "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM";  
// char ssid[] = "LLHTDQC@+";               
// char pass[] = "bfbfbfbf";               
char ssid[] = "WIFI_NGU_NHU_BO";           
char pass[] = "khongcomatkhau123@";        

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    dht.begin();

    Wire.begin(SDA, SCL);

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("ESP8266_BLYNKIOT");
    lcd.setCursor(0, 1);
    lcd.print("DANG KHOI TAO...");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    Blynk.virtualWrite(V3, LOW);
    Blynk.virtualWrite(V4, LOW);

    delay(2000);
    lcd.clear();
}

void loop() {
    Blynk.run();
    updateSensorData();
    delay(1000);
}

void updateSensorData() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temperature) && !isnan(humidity)) {
        // BLYNK
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);

        if (userControll && (millis() - lastControll >= 10000)) {
            userControll = false;  // Quay lại chế độ tự động
        }

        // userControll: tự động bật/tắt đèn dựa theo nhiệt độ
        if (!userControll) {
          if (temperature <= 22) {
              ledState = HIGH;  // Bật LED
          } else {
              ledState = LOW;   // Tắt LED
          }
          digitalWrite(LED_PIN, ledState);
          Blynk.virtualWrite(V4, ledState);
          Blynk.virtualWrite(V3, ledState);
        }

        // LCD
        lcd.setCursor(0, 0);
        lcd.print("T:" + String(temperature) + "-");
        lcd.setCursor(8, 0);
        lcd.print("H:" + String(humidity));
        lcd.setCursor(0, 1);
        lcd.print("LED: " + String(ledState ? "ON " : "OFF"));
    }
}

// bật/tắt công tắc đèn thủ công
BLYNK_WRITE(V3) {
    userControll = true;  // Người dùng đang can thiệp
    lastControll = millis();  // Lưu thời gian gần nhất mà người dùng nhấn nút
    ledState = param.asInt();  
    digitalWrite(LED_PIN, ledState);
    Blynk.virtualWrite(V4, ledState); 
}
