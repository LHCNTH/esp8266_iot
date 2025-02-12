// #define BLYNK_TEMPLATE_ID "TMPL6aXoA2tT-"
// #define BLYNK_TEMPLATE_NAME "Do An 3"
// #define BLYNK_AUTH_TOKEN "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM"

// #include <ESP8266WiFi.h>
// #include <BlynkSimpleEsp8266.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <DHT.h>

// // *** Khai báo chân I2C cho LCD ***
// #define SDA 4  // GPIO4 (D2)
// #define SCL 5  // GPIO5 (D1)

// // *** Khai báo LCD 16x2 với địa chỉ I2C ***
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// // *** Cấu hình cảm biến DHT11 ***
// #define DHTPIN 2  // Chân DHT11 nối với GPIO2 (D4)
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // *** Cấu hình LED mini 5V ***
// #define LED_PIN 0  // Chân GPIO5 (D3)

// char auth[] = "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM";  // Nhập mã Auth Token từ Blynk
// char ssid[] = "LLHTDQC@+";               // Nhập tên WiFi
// char pass[] = "bfbfbfbf";               // Nhập mật khẩu WiFi

// void setup() {
//     Serial.begin(9600);
//     Blynk.begin(auth, ssid, pass);

//     dht.begin();

//     // *** Khởi tạo I2C với chân SDA & SCL ***
//     Wire.begin(SDA, SCL);

//     // *** Khởi tạo LCD ***
//     lcd.init();
//     lcd.backlight();

//     // *** Hiển thị thông báo ban đầu ***
//     lcd.setCursor(0, 0);
//     lcd.print("ESP8266 + BLYNK");
//     lcd.setCursor(0, 1);
//     lcd.print("DANG KHOI TAO...");
//     delay(5000);

//     pinMode(LED_PIN, OUTPUT);
//     digitalWrite(LED_PIN, LOW);  
// }

// void loop() {
//     Blynk.run();
//     updateSensorData();
//     delay(2000);
// }

// // *** Đo nhiệt độ, độ ẩm & cập nhật dữ liệu ***
// void updateSensorData() {
//     float temperature = dht.readTemperature();
//     float humidity = dht.readHumidity();

//     if (!isnan(temperature) && !isnan(humidity)) {
//         // Gửi dữ liệu lên Web Blynk
//         Blynk.virtualWrite(V1, temperature);
//         Blynk.virtualWrite(V2, humidity);

//         // Hiển thị trên LCD 16x2
//         lcd.clear();
//         lcd.setCursor(0, 0);
//         lcd.print("ND:" + String(temperature) + " - ");
//         lcd.setCursor(7, 0); 
//         lcd.print("DA:" + String(humidity));
//     }
// }

// // *** Nhận tín hiệu từ Web Blynk (V3) để bật/tắt đèn ***
// BLYNK_WRITE(V3) {
//     int ledState = param.asInt();

//     digitalWrite(LED_PIN, ledState);
//     Blynk.virtualWrite(V4, ledState);

//     lcd.setCursor(0, 1);
//     lcd.print("LED: " + String(ledState ? "ON " : "OFF"));
// }



#define BLYNK_TEMPLATE_ID "TMPL6aXoA2tT-"
#define BLYNK_TEMPLATE_NAME "Do An 3"
#define BLYNK_AUTH_TOKEN "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// *** Khai báo chân I2C cho LCD ***
#define SDA 4  // GPIO4 (D2)
#define SCL 5  // GPIO5 (D1)

// *** Khai báo LCD 16x2 với địa chỉ I2C ***
LiquidCrystal_I2C lcd(0x27, 16, 2);

// *** Cấu hình cảm biến DHT11 ***
#define DHTPIN 2  // Chân DHT11 nối với GPIO2 (D4)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// *** Cấu hình LED mini 5V ***
#define LED_PIN 0  // Chân GPIO0 (D3)

// *** Biến trạng thái LED ***
int ledState = LOW;

char auth[] = "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM";  // Nhập mã Auth Token từ Blynk
// char ssid[] = "LLHTDQC@+";               // Nhập tên WiFi
// char pass[] = "bfbfbfbf";               // Nhập mật khẩu WiFi
char ssid[] = "WIFI_NGU_NHU_BO";               // Nhập tên WiFi
char pass[] = "khongcomatkhau123@";               // Nhập mật khẩu WiFi

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);

    dht.begin();

    // *** Khởi tạo I2C với chân SDA & SCL ***
    Wire.begin(SDA, SCL);

    // *** Khởi tạo LCD ***
    lcd.init();
    lcd.backlight();

    // *** Hiển thị thông báo ban đầu ***
    lcd.setCursor(0, 0);
    lcd.print("ESP8266 + BLYNK");
    lcd.setCursor(0, 1);
    lcd.print("DANG KHOI TAO...");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);

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
        // LCD
        lcd.setCursor(0, 0);
        lcd.print("T:" + String(temperature) + "-");
        lcd.setCursor(8, 0);
        lcd.print("H:" + String(humidity));
        lcd.setCursor(0, 1);
        lcd.print("LED: " + String(ledState ? "ON " : "OFF"));
    }
}

// *** Nhận tín hiệu từ Web Blynk (V3) để bật/tắt đèn ***
BLYNK_WRITE(V3) {
    ledState = param.asInt();  // Nhận giá trị từ Web Blynk
    digitalWrite(LED_PIN, ledState);
    Blynk.virtualWrite(V4, ledState); 
}
