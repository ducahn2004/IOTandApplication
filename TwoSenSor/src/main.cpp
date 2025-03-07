// **Thông tin Blynk**
#define BLYNK_TEMPLATE_ID "TMPL6w9JB5Jxc"
#define BLYNK_TEMPLATE_NAME "TwoSensorProject"
#define BLYNK_AUTH_TOKEN "QUSuEQAmlq3BE17vVI9IH8t6wtzymUmT"


#include <Arduino.h>
// Thư viện cần thiết
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>



// **Thông tin WiFi**
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// **Khai báo cảm biến DHT22**
#define DHTPIN 4       // Chân kết nối DHT22 (GPIO4)
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// **Khai báo cảm biến quang trở (LDR)**
const float GAMMA = 0.7;
const float RL10 = 50.0;
#define LDRPIN 34  // ESP32 ADC GPIO34

// **Chân LED cảnh báo nhiệt độ**
#define LEDPIN 2  // Chân GPIO2 (D2)

void setup() {
    Serial.begin(115200);
    
    // **Kết nối WiFi**
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    // **Khởi động Blynk**
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    // **Cấu hình cảm biến & LED**
    dht.begin();
    pinMode(LDRPIN, INPUT);
    pinMode(LEDPIN, OUTPUT);
}

void loop() {
    Blynk.run();  // Chạy Blynk
    
    // **Đọc dữ liệu từ DHT22**
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // **Kiểm tra dữ liệu hợp lệ**
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Lỗi đọc dữ liệu từ DHT22!");
        return;
    }

    // **Gửi dữ liệu DHT22 lên Blynk**
    Blynk.virtualWrite(V0, temperature);  // Gửi nhiệt độ lên V0
    Blynk.virtualWrite(V1, humidity);     // Gửi độ ẩm lên V1

    // **Hiển thị trên Serial**
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    // **Bật LED nếu nhiệt độ quá cao**
    if (temperature > 30.0) {
        digitalWrite(LEDPIN, HIGH);
    } else {
        digitalWrite(LEDPIN, LOW);
    }

    // **Đọc dữ liệu từ cảm biến quang trở (LDR)**
    int analogValue = analogRead(LDRPIN);  // ESP32 dùng ADC 12-bit (0 - 4095)
    float voltage = analogValue / 4095.0 * 3.3;
    float resistance = 2000 * voltage / (3.3 - voltage);
    float lux = pow((RL10 * 1e3 * pow(10, GAMMA) / resistance), (1.0 / GAMMA));

    // **Gửi giá trị ánh sáng lên Blynk**
    Blynk.virtualWrite(V2, lux);  // Gửi giá trị Lux lên V2

    // **Hiển thị trên Serial**
    Serial.print("Light Intensity: ");
    Serial.print(lux);
    Serial.println(" lux");

    // **Chờ 2 giây trước khi đọc lại**
    delay(1000);
}
