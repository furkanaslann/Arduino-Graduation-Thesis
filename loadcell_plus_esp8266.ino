#include "HX711.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "Galaxy M31s";      // WiFi ağ adı
const char *password = "mhtb4111";  // WiFi şifresi
const char *api_key = "OF81N1WTUDJ1PMLC";  // ThingSpeak API anahtarı
const char *api_write = "api.thingspeak.com";
const char *channel_id = "2390979";  // ThingSpeak Channel ID

HX711 scale;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  scale.begin(1, 2);  // HX711 pin bağlantıları (DT=1, SCK=2)

  // ThingSpeak API'ye gönderilen veri formatı
  // https://api.thingspeak.com/update?api_key=YOUR_API_KEY&field1=SENSOR_VALUE
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Sensörden ağırlık okuma
    float weight = scale.get_units(10);

    if (!isnan(weight)) {
      Serial.print("Weight: ");
      Serial.println(weight);

      // ThingSpeak'e veri gönderme
      String url = "/update?";
      url += "api_key=";
      url += api_key;
      url += "&field1=";
      url += String(weight, 2);

      String fullUrl = "/channels/";
      fullUrl += channel_id;
      fullUrl += url;

      HTTPClient http;
      http.begin(api_write, 80, fullUrl, "");
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("Error reading from the scale");
    }

    delay(15000);  // 15 saniye bekleme
  } else {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.begin(ssid, password);
  }
}
