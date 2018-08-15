/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "wel";
const char *password = "password";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  server.setNoDelay(true);
  Serial.println("Server is hippin and hoppin and it won't be stoppin");
}

void loop() {
  // Check if a client has connected
   WiFiClient client = server.available();
   delay(500);

  if (client){
    Serial.println("Client connected");
    Serial.println(client.available());
    if(client.available() != 0){
      Serial.print("Bene says: "); 
      Serial.println(client.readStringUntil('\n'));
    }
    client.flush();
    while(true){
      server.println("This is really awkward...");
      delay(50);
    }
  }

  while(client){
    Serial.println(client.connected());
    Serial.print("Bene says: "); 
    Serial.println(client.readStringUntil('\n'));
  }
}
