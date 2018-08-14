/*
Geekstips.com
IoT project - Communication between two ESP8266 - Talk with Each Other
ESP8266 Arduino code example
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>2

// Button Declarations
int button = 16; // push button is connected
int temp = 0;    // temporary variable for reading the button pin status


// AP Wi-Fi credentials
const char* ssid = "Sender_01";
const char* password = "abcdef123456!";
  
// Local ESP web-server address
String highHost = "http://192.168.4.1/high";
String lowHost = "http://192.168.4.1/low";
String data;
// DEEP_SLEEP Timeout interval
int sleepInterval = 5;
// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2;
int counter = 0;

// Static network configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

void setup() {
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.begin(115200);
  Serial.println();
  Serial.println("**************************");
  Serial.println("**************************");
  Serial.println("******** BEGIN ***********");
  delay(500);
  Serial.println("- set ESP STA mode");
  WiFi.mode(WIFI_STA);
  Serial.println("- connecting to wifi");
  //WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    if(counter > 20){
       Serial.println("- can't connect, going to sleep");
    }
    delay(500);
    Serial.print(".");
    counter++;
  }
  
  Serial.println("- wifi connected");
  
}

void sendHttpRequest() {
  HTTPClient http;
  if(temp==HIGH){
    http.begin(highHost);  
  } else {
    http.begin(lowHost);
  }
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
}

void buildDataStream() {
  data = "temp=";
  data += String(temp);
  Serial.println("- data stream: "+data);
}

void loop() {
  Serial.println("- read Button");
  temp = digitalRead(button);
  Serial.println("- build DATA stream string");
  //buildDataStream();
  Serial.println("- send GET request");
  sendHttpRequest();
  Serial.println();
  Serial.println("- sent");
  Serial.println("**************************");
  Serial.println("**************************");
}
