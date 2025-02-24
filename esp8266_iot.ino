
#define BLYNK_TEMPLATE_ID "TMPL6aXoA2tT-"
#define BLYNK_TEMPLATE_NAME "Do An 3"
#define BLYNK_AUTH_TOKEN "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>

#define SDA 4  // GPIO4 (D2)
#define SCL 5  // GPIO5 (D1)
#define DHTPIN 2  // Chân DHT11 nối với GPIO2 (D4)
#define DHTTYPE DHT11
#define LED_PIN 0  // Chân GPIO0 (D3)
#define FAN_PIN 15 // Chân GPIO15 (D8)

#define SCLK 14 // GPIO14 (D5) - Clock
#define RCLK 12 // GPIO12 (D6) - Latch
#define DIO  13 // GPIO13 (D7) - Data

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

int ledState = LOW;
int fanState = HIGH;
bool userControllLed = false;
bool userControllFan = false;
unsigned long lastControllLed = 0;
unsigned long lastControllFan = 0;

// Biến lưu thời gian chuyển đổi hiển thị
unsigned long lastDisplaySwitch = 0;
bool showTime = true;  // true: hiển thị thời gian, false: hiển thị nhiệt độ, độ ẩm

char auth[] = "ShTQMdWYP9U-vNo-tpdQ0XsX5t7K2NBM";  
char ssid[] = "WIFI_NGU_NHU_BO";           
char pass[] = "khongcomatkhau123@";   
//char ssid[] = "LLHTDQC@+";           
//char pass[] = "bfbfbfbf";        

// Bảng mã hiển thị số trên LED 7 đoạn
int hexDigitValue[] = {
    0xFC,    // 0
    0x60,    // 1
    0xDA,    // 2
    0xF2,    // 3
    0x66,    // 4
    0xB6,    // 5
    0xBE,    // 6
    0xE0,    // 7
    0xFE,    // 8
    0xF6     // 9
};

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    dht.begin();
    Wire.begin();
    rtc.begin();

    if (!rtc.begin()) {
        Serial.println("Khong tim thay Module RTC!");           
    }
    if (rtc.lostPower()) {
        Serial.println("Dang thiet lap thoi gian hien tai.");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Thiết lập thời gian hiện tại
    }

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("ESP8266_BLYNKIOT");
    lcd.setCursor(0, 1);
    lcd.print("DANG KHOI TAO...");

    pinMode(LED_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SCLK, OUTPUT);  
    pinMode(DIO, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    digitalWrite(FAN_PIN, fanState);
    Blynk.virtualWrite(V3, LOW);
    Blynk.virtualWrite(V4, LOW);
    Blynk.virtualWrite(V5, LOW);

    delay(2000);
    lcd.clear();
}

// Hiển thị một digit tại vị trí row
void setDigit(unsigned int row, unsigned int digit, boolean decimalPoint) {
    unsigned int rowSelector = bit(3 - row) << 4;
    unsigned int data = ~hexDigitValue[digit & 0xF];

    if (decimalPoint) {
        data &= 0xFE;
    }

    digitalWrite(RCLK, LOW);
    shiftOut(DIO, SCLK, LSBFIRST, data);
    shiftOut(DIO, SCLK, LSBFIRST, rowSelector);
    digitalWrite(RCLK, HIGH);
}

// Hiển thị thời gian HH:MM
void displayTime7SEG(int hour, int minute, bool showColon) {
    int digits[4] = {
        minute % 10,      
        minute / 10,      
        hour % 10,        
        hour / 10         
    };

    for (int i = 0; i < 4; i++) {
        setDigit(i, digits[i], (i == 2) ? showColon : false);
        delay(2);
    }
}

void onDisplay7SEG(){
    static unsigned long lastBlink = 0;
    static bool showColon = false;
    DateTime now = rtc.now();  // Lấy thời gian hiện tại từ RTC
    now = now + TimeSpan(0, 0, 0, 32); // Cộng thêm 32 giây
    if (millis() - lastBlink >= 1000) {  
        showColon = !showColon;  
        lastBlink = millis();
    }
    displayTime7SEG(now.hour(), now.minute(), showColon);
}

unsigned long lastLCDSwitch = 0;
const unsigned long lcdSwitchInterval = 10000;  
unsigned long lastSecondUpdate = 0;
const unsigned long secondUpdateInterval = 1000;  
bool lastFanState = true;  // Trạng thái trước đó của quạt
bool lastLightState = false;  // Trạng thái trước đó của đèn
void loop() {
    Blynk.run();
    onDisplay7SEG();  // Luôn cập nhật LED 7 đoạn
    DateTime now = rtc.now();
    now = now + TimeSpan(0, 0, 0, 32);

    // Kiểm tra xem có cần chuyển đổi hiển thị LCD không (mỗi 5s)
    if (millis() - lastLCDSwitch >= lcdSwitchInterval) {
        lastLCDSwitch = millis();
        showTime = !showTime;
        lcd.clear();
        // lcd.setCursor(0, 0);
        // lcd.print("                ");  // Xóa hàng trên
        // lcd.setCursor(0, 1);
        // lcd.print("                ");  // Xóa hàng dưới
                                    
        if (showTime) {
            displayTimeLCD();
        } else {
            updateSensorData();  // Hiển thị nhiệt độ, độ ẩm, trạng thái quạt, đèn
        }
    }

    // Cập nhật số giây trên LCD khi đang hiển thị thời gian
    if (showTime && millis() - lastSecondUpdate >= secondUpdateInterval) {
        lastSecondUpdate = millis();
        lcd.setCursor(12, 1);
        if (now.second() < 10) lcd.print("0");
        lcd.print(now.second());
        lcd.print("  ");
    }
    // Kiểm tra trạng thái quạt/đèn từ Blynk để cập nhật lên LCD ngay khi có thay đổi
    bool fanState = digitalRead(FAN_PIN);
    bool lightState = digitalRead(LED_PIN);
    if (fanState != lastFanState || lightState != lastLightState) {
        lastFanState = fanState;
        lastLightState = lightState;
        if (!showTime) {  // Nếu đang hiển thị trạng thái cảm biến
            lcd.setCursor(0, 1);
            lcd.print("LED:");
            lcd.print(lightState ? "ON " : "OFF");
            lcd.setCursor(9, 1);
            lcd.print("FAN:");
            lcd.print(fanState ? "OFF" : "ON ");
        }
    }
}

// Hiển thị thời gian thực lên LCD
void displayTimeLCD() {
    DateTime time = rtc.now(); 
    DateTime newTime = time + TimeSpan(0, 0, 0, 32);

    int day = time.day();
    int month = time.month();
    int year = time.year();
    int hour = newTime.hour();
    int minute = newTime.minute();
    int second = newTime.second();

    // Hiển thị ngày tháng năm (DD-MM-YYYY)
    lcd.setCursor(0, 0);
    lcd.print("DATE: ");
    lcd.print((day < 10) ? "0" : "");  
    lcd.print(day);
    lcd.print("-");
    lcd.print((month < 10) ? "0" : "");
    lcd.print(month);
    lcd.print("-");
    lcd.print(year);

    // Hiển thị giờ phút giây (HH:MM:SS)
    lcd.setCursor(0, 1);
    lcd.print("TIME: ");
    lcd.print((hour < 10) ? "0" : "");  
    lcd.print(hour);
    lcd.print(":");
    lcd.print((minute < 10) ? "0" : "");
    lcd.print(minute);
    lcd.print(":");
    lcd.print((second < 10) ? "0" : "");
    lcd.print(second);
}

// Hiển thị nhiệt độ, độ ẩm và trạng thái thiết bị lên LCD
void updateSensorData() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
        
    if (!isnan(temperature) && !isnan(humidity)) {
        // Gửi dữ liệu lên Blynk
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);

        if (userControllLed && (millis() - lastControllLed >= 600000)) {
            userControllLed = false;  // Quay lại chế độ tự động
        }

        if (userControllFan && (millis() - lastControllFan >= 600000)) {
            userControllFan = false;  // Quay lại chế độ tự động
        }

        // Điều khiển LED theo nhiệt độ nếu không có sự can thiệp từ người dùng
        if (!userControllLed) {
          if (temperature <= 23) {
              ledState = HIGH;  // Bật LED
          } else {
              ledState = LOW;   // Tắt LED
          }
          digitalWrite(LED_PIN, ledState);
          Blynk.virtualWrite(V4, ledState);
          Blynk.virtualWrite(V3, ledState);
        }

        // Điều khiển Quạt theo nhiệt độ nếu không có sự can thiệp từ người dùng
        if (!userControllFan) {
          if (temperature > 25) {
              fanState = LOW;  // Bật quạt
          } else {
              fanState = HIGH;   // Tắt quạt
          }
          digitalWrite(FAN_PIN, fanState);
          Blynk.virtualWrite(V5, fanState);
        }

        // Hiển thị thông tin lên LCD
        lcd.setCursor(0, 0);
        lcd.print("T:" + String(temperature));
        lcd.setCursor(9, 0);
        lcd.print("H:" + String(humidity));

        lcd.setCursor(0, 1);
        lcd.print("LED:");
        lcd.print(ledState == HIGH ? "ON " : "OFF");

        lcd.setCursor(9, 1);
        lcd.print("FAN:");
        lcd.print(fanState == LOW ? "ON " : "OFF");
    }
}

// Bật/tắt LED thủ công
BLYNK_WRITE(V3) {
    userControllLed = true;  // Người dùng can thiệp
    lastControllLed = millis();  // Lưu thời gian nhấn nút
    ledState = param.asInt();  
    digitalWrite(LED_PIN, ledState);
    Blynk.virtualWrite(V4, ledState); 
}

// Bật/tắt Quạt thủ công
BLYNK_WRITE(V5) {
    userControllFan = true;  // Người dùng can thiệp
    lastControllFan = millis();  // Lưu thời gian nhấn nút
    fanState = param.asInt();  
    digitalWrite(FAN_PIN, fanState);
    Blynk.virtualWrite(V5, fanState); 
}

