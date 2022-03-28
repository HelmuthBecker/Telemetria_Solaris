#include "WiFi.h"
#include "ESPAsyncWebServer.h"
 
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

IPAddress local_IP(192,168,1,60);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
 
AsyncWebServer server(80);
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/info1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "11.1|22.2|33.3|44.4");
  });

  server.on("/info2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "55.5|66.6|77.7|88.8");
  });
 
  server.begin();
}
 
void loop(){}
