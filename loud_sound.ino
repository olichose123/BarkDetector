#include <M5Unified.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Credentials.h>

const char* ssid = SSID;
const char* password = PASSWORD;
String serverUrl = SERVER_ENDPOINT;

static constexpr const size_t record_number = 256;
static constexpr const size_t record_length = 200;
static constexpr const size_t record_size = record_number * record_length;
static constexpr const size_t record_samplerate = 16000;
static size_t rec_record_idx = 2;
static size_t draw_record_idx = 0;
static int16_t *rec_data;
static int16_t cooldown = 0;
const int16_t VOLUME_THRESHOLD = VOLUME_THRESHOLD;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.println("Wifi");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  rec_data = (typeof(rec_data))heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_8BIT);
  memset(rec_data, 0 , record_size * sizeof(int16_t));
  M5.Mic.begin();
}

void loop() {
  M5.update();
  if (M5.Mic.isEnabled() && WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    
    static constexpr int shift = 6;
    int16_t* current_audio_chunk = &rec_data[rec_record_idx * record_length];
    if (M5.Mic.record(current_audio_chunk, record_length, record_samplerate))
    {
      int16_t peak_amplitude = 0;
      for (size_t i = 0; i < record_length; ++i) {
        int16_t sample_abs_value = std::abs(current_audio_chunk[i]);
        if (sample_abs_value > peak_amplitude) {
          peak_amplitude = sample_abs_value;
        }
      }
      if (cooldown > 0)
      {
        cooldown--;
      }
      
      if (peak_amplitude > VOLUME_THRESHOLD && cooldown <= 0) {
        Serial.printf("ALERT! Peak amplitude %d exceeds threshold %d. Sending HTTP POST to %s\n", 
                      peak_amplitude, VOLUME_THRESHOLD, serverUrl.c_str());
        cooldown = DELAY;

        HTTPClient http;
        http.begin(serverUrl);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.printf("HTTP POST successful. Response code: %d\n", httpResponseCode);
        } else {
            Serial.printf("Error sending HTTP POST. Code: %d, Error: %s\n", 
                          httpResponseCode, http.errorToString(httpResponseCode).c_str());
        }
        http.end();
      }
      if (++rec_record_idx >= record_number) { rec_record_idx = 0; }
    }
  }
  delay(5);
}
