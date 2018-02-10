#include <DHT.h>
#include <DHT_U.h>
#include <TM1637Display.h>
#include <RtcDS3231.h>  //RTC library
#include <MicroGear.h>
#include <ESP8266WiFi.h>

const char* ssid     = "kidbright";
const char* password = "12345678";

#define APPID   "envobserve"
#define KEY     "yjVQ7Jg0dRZgHcB"
#define SECRET  "RMFKAaccrPGbsht08Bq7nivMU"

#define ALIAS   "NodeMCU1"
#define TargetWeb "HTML_web"

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN D3     // what digital pin we're connected to
#define CLK D6     // Set the CLK pin connection to display
#define DIO D5      // Set the DIO pin connection to display

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

int numCounter = 0;
TM1637Display display(CLK, DIO); //Setup the 4-digit Display

//RtcDS3231 rtcObject;  // RTC Setup

WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}


void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() {
  Serial.begin(250000);
  Serial.println("DHTxx test!");

  /* Event listener */
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);

    Serial.println("NETPIE Connected : Done... ");

  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);

  dht.begin();
  
  //setup 4 digits display
  display.setBrightness(0x0a); //set display to maximum brightness
  
}

void loop() {
  // Wait a few seconds between measurements.
  //digitalWrite(LED_BUILTIN, LOW);
  //delay(1000);
  //digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

 /* Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

 */
  display.showNumberDec(int(t)+int(h)*100); //Display the numCounter value;

  if (microgear.connected())
    {
       microgear.loop();
       Serial.println("connected");

       float Humidity = dht.readHumidity();
       float Temp = dht.readTemperature();  // Read temperature as Celsius (the default)
       String data = String(Humidity) + "," + String(Temp);
      
       char msg[128];
       data.toCharArray(msg,data.length());
       Serial.println(msg);    

       microgear.chat(TargetWeb , msg);
    }
   else 
   {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
   }

}
