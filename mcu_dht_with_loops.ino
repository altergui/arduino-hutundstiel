#include <ESP8266WiFi.h>
 
const char* ssid = "HUS_Fruchtraum_2ghz";
const char* password = "XXXXXXXXXX";
 
WiFiServer server(80);

#include "DHT.h"

#define DHTTYPE DHT22   // Options are 'DHT11', 'DHT22' (AM2302) or 'DHT21' (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// NodeMCU pinout according to https://iotbytes.wordpress.com/nodemcu-pinout/

#define DHTPIN1 5     // "D1" on PCB
#define DHTPIN2 4     // "D2" on PCB
#define DHTPIN3 0     // "D3" on PCB
#define DHTPIN4 2     // "D4" on PCB

#define RELAYPIN1 14     // "D5" on PCB
#define RELAYPIN2 12     // "D6" on PCB

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);
DHT dht4(DHTPIN4, DHTTYPE);

DHT dht[4] = {dht1, dht2, dht3, dht4};

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RELAYPIN1, OUTPUT); // relay1 (air conditioner or heating)
  pinMode(RELAYPIN2, OUTPUT); // relay2 (humidifier)

  pinMode(LED_BUILTIN, OUTPUT); // status LED

  for(int i = 0; i < 4; i++) {
    dht[i].begin();   // init sensors
  }

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("### Will try to connect to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);

  // Start the server
  server.begin();
  Serial.println("### Server started");
}
 
void loop() {
  
  delay (4000); // 1 second is spent reading each sensor (250ms each)
                // so total time between reads is ~5 secs

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  float h[4];
  float t[4];
  
  for (int i = 0; i < 4; i++) {
    h[i] = dht[i].readHumidity();
    t[i] = dht[i].readTemperature();

    Serial.print("hum"); 
    Serial.print(i); 
    Serial.print("="); 
    Serial.print(h[i]);
    Serial.print(";"); 
    Serial.print("temp"); 
    Serial.print(i); 
    Serial.print("="); 
    Serial.print(t[i]);
    Serial.print(";"); 
  }
  
  Serial.println();

  // Take actions depending on conditions of sensor number 0

  if (t[0] < 50.0) {              // Temp is low (normal)
    digitalWrite(RELAYPIN1,HIGH); // Turn relay 1 OFF
  } 

  if (t[0] >= 50.0 ){            // Temp is too high
    digitalWrite(RELAYPIN1,LOW); // Turn relay 1 ON
  }

  if (h[0] <= 88.0){             // Humidity is below 88%
    digitalWrite(RELAYPIN2,LOW); // Turn relay 2 ON
  } 

  if (h[0] > 92.0){               // Humidity is above 92%
    digitalWrite(RELAYPIN2,HIGH); // Turn relay 2 OFF
  } 

  // Humidity between 88% and 92% will not change relay 2 state (if it was ON, stays ON)

  if (WiFi.status() != WL_CONNECTED) { // If WiFi Client is still not connected
    digitalWrite(LED_BUILTIN, HIGH);        // blink status LED twice in a window of 250ms
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("### new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.print("### request:");
  Serial.println(request);
  client.flush();
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one

  // Output the temp and hum data in same format as serial, but over HTTP
  for (int i = 0; i < 4; i++) {
    client.print("hum"); 
    client.print(i); 
    client.print("="); 
    client.print(h[i]);
    client.print(";"); 
    client.print("temp"); 
    client.print(i); 
    client.print("="); 
    client.print(t[i]);
    client.print(";"); 
  }
 
  delay(1);
  Serial.println("### Client disconnected");
  Serial.println("");
}
