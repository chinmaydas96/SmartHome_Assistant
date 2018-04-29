/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  
  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <Servo.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// the on off button feed turns this LED on/off
#define LED D0
// the slider feed sets the PWM output of this pin
#define PWMOUT D6
#define MOTORPIN D7
Servo myservo;

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "XNet"
#define WLAN_PASS       "arpanet1234"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "debasis1"
#define AIO_KEY         "d85e000da37f41928ec89a548957de52"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light");
Adafruit_MQTT_Subscribe ledBrightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledBrightness");
Adafruit_MQTT_Subscribe blind = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/blind");
Adafruit_MQTT_Subscribe fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fan");
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(PWMOUT, OUTPUT);
  pinMode(MOTORPIN, OUTPUT);
  myservo.attach(2);

  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff & slider feed.
  mqtt.subscribe(&light);
  mqtt.subscribe(&ledBrightness);
  mqtt.subscribe(&blind);
  mqtt.subscribe(&fan);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  int pos;

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    // Check if its the onoff button feed
    if (subscription == &light) {
      Serial.print(F("On-Off button: "));
      Serial.println((char *)light.lastread);
      
      if (strcmp((char *)light.lastread, "ON") == 0) {
        digitalWrite(LED, LOW); 
      }
      if (strcmp((char *)light.lastread, "OFF") == 0) {
        digitalWrite(LED, HIGH); 
      }
    }
    
    // check if its the slider feed
    if (subscription == &ledBrightness) {
      Serial.print(F("Slider: "));
      Serial.println((char *)ledBrightness.lastread);
      uint16_t sliderval = atoi((char *)ledBrightness.lastread);  // convert to a number
      analogWrite(PWMOUT, sliderval);
    }
 //for servo motor
    if (subscription == &blind) {
      Serial.print(F("blind Open-close button: "));
      Serial.println((char *)blind.lastread);
      
      if (strcmp((char *)blind.lastread, "ON") == 0) {
          for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
          {                                  // in steps of 1 degree 
           myservo.write(pos);              // tell servo to go to position in variable 'pos' 
          } 
         }
      if (strcmp((char *)blind.lastread, "OFF") == 0) {
         for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
          {                                
           myservo.write(pos);              // tell servo to go to position in variable 'pos'  
          } 
       }
    }
  
    if (subscription == &fan) {
      Serial.print(F("FAN SPEED: "));
      Serial.println((char *)fan.lastread);
      uint16_t speedval = atoi((char *)fan.lastread);  // convert to a number
      analogWrite(MOTORPIN, speedval);
    }
    
  }

  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
code_org.txt
Displaying code_org.txt.