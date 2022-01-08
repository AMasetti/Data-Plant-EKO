#include <Adafruit_Sensor.h>
#include <FastLED.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// MQTT Device ID definitions
#define DEVICE "eko"          // Device Name DataPlantEKO
#define ROOT "iotplant"       // MQTT Topic root

// Pin definitions
#define DHTPIN 26             // Digital pin connected to the DHT sensor
#define NUM_LEDS 8            // Number of LEDs in the strip
#define NEOPIN 25             // PWM Pin for the Neopixel ring
#define BRIGHTNESS  100       // Brightness of the LED strip
#define LIGHTSENSORPIN 39     // Ambient light sensor pin
#define SOILHUMPIN 36         // Soil humidity sensor pin

// Sensors definitions
#define DHTTYPE DHT11          // DHT 11 type of sensor in use (11, 21, 22)

// Client declarations
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[100];
int value = 0;

// Hardware declarations
DHT dht(DHTPIN, DHTTYPE);
CRGB leds[NUM_LEDS];

// MQTT Topics definitions
String topic_root =  String(ROOT) + "/" + String(DEVICE);
// Data topic
String publish_data_str = topic_root + "/data";
const char* publish_data = publish_data_str.c_str();
// Configurations topic
String subs_config_str = topic_root + "/config";
const char* subs_config = subs_config_str.c_str();
// Neopixel status topic
String subs_neopixels_str = topic_root + "/neopixels";
const char* subs_neopixels = subs_neopixels_str.c_str();
// Reset alert topic
String publish_reset_string = topic_root + "/reset";
const char* publish_reset = publish_reset_string.c_str();

// NTP
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 

// Plant configuration variables
bool enableMQTT = true;           // Sets the MQTT Client to ON by default
int sample_time = 500;            // Sample time in milliseconds
float minTempDHT = 0;             // Minimum temperature to be considered as a healthy plant (celsius)
float maxTempDHT = 50;            // Maximum temperature to be considered as a healthy plant (celsius)
float minHumDHT  = 0;             // Minimum humidity to be considered as a healthy plant (percentage)
float maxHumDHT  = 100;           // Maximum humidity to be considered as a healthy plant (percentage)
float minLight = 0;               // Minimum light to be considered as a healthy plant (percentage)
float maxLight = 100;             // Maximum light to be considered as a healthy plant (percentage)
float minSoilHum = 0;             // Minimum soil humidity to be considered as a healthy plant (percentage)
float maxSoilHum = 100;           // Maximum soil humidity to be considered as a healthy plant (percentage)
float minSoilTemp = 0;            // Minimum soil temperature to be considered as a healthy plant (celsius)
float maxSoilTemp = 50;           // Maximum soil temperature to be considered as a healthy plant (celsius)


unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == String(subs_neopixels)) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
//      fill_solid( leds, NUM_LEDS, CRGB::Black);
//      delay(50);
//      FastLED.show();
//      delay(50);
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {

//      fill_solid( leds, NUM_LEDS, CRGB::Blue);
//      delay(50);
//      FastLED.show();
//      delay(50);
    }
  }
}

void neo_wait() {
  // Flash LEDs in a circle with dimming brightness
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(50);
  }
}

void neo_ok() {
  // pulsate the LEDs
  fill_solid( leds, NUM_LEDS, CRGB::Green);
  for (int i = 0; i < 100; i++) {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(10);
  }
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  for (int i = 100; i > 0; i--) {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(10);
  }
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(subs_neopixels);
     client.subscribe(subs_config);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Configuring the device...");
  
  dht.begin();
  Serial.println("DHT11 Sensor configured on pin " + String(DHTPIN));
  
  FastLED.addLeds<NEOPIXEL, NEOPIN>(leds, NUM_LEDS);
  Serial.println("Neopixels configured on pin " + String(NEOPIN));

  setup_wifi();
  Serial.println("WiFi configured");

  if (enableMQTT) {
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
    Serial.println("MQTT Client configured");
  }

  configTime(-3, 0, ntpServer);
  Serial.println("NTP configured");

  Serial.println("Configuration complete");
   delay(5000);
  neo_wait();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > sample_time) {
    lastMsg = now;
    
    // Read DHT temperature
    float dht_temp = dht.readTemperature();
    if (isnan(dht_temp)) {
      Serial.println("Failed to read from DHT sensor!");
    }

    // Read DHT humidity
    float dht_hum = dht.readHumidity();
    if (isnan(dht_hum)) {
      Serial.println("Failed to read from DHT sensor!");
    }

    // Read soil humidity in percentage
    int soil_hum = analogRead(SOILHUMPIN);
    soil_hum = map(soil_hum, 4095, 0, 0, 100);

    // Read light level
    float reading = analogRead(LIGHTSENSORPIN);
    float lux = map(reading, 0, 4095, 0, 1000);


    // Touch Sensor
    bool touch = touchRead(32) < 70;
    if (touch) {
      neo_ok();
    }

    Serial.println("------------------------------------------------------");
    Serial.print("DHT Temp Sensor: ");
    Serial.println(dht_temp);
    Serial.print("DHT Humidity Sensor: ");
    Serial.println(dht_hum);
    Serial.print("Soil Humidity Sensor: ");
    Serial.println(soil_hum);
    Serial.print("Light Sensor: ");
    Serial.println(lux);
    Serial.print("Touch Sensor: ");
    Serial.println(touch);
    
    Serial.print("Sent: ");

    // Create the payload to send
    sprintf(msg, "{\"dht_temp\":%2f, \"dht_hum\":%2f,\"light\":%2f,\"soil_hum\":%d}", dht_temp, dht_hum, lux, soil_hum);
    client.publish(publish_data, msg);

    epochTime = getTime();
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
  }
}
