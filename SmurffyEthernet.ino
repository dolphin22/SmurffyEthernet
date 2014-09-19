#include <avr/wdt.h>
#include <stdlib.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LibHumidity.h>

LibHumidity sht25 = LibHumidity(0);

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  
EthernetClient client;

IPAddress server(128,199,246,241); 
//char serverName[] = "ismurffy.com";
unsigned static int port = 49157;

char deviceID[10] = "S001";

void setup() {
  Serial.begin(9600);
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("DHCP failed"));
    for(;;) {
      ;
    }
  }
  
  Serial.println(Ethernet.localIP());
  
  connectToServer();
  
  // Enable watchdog
  wdt_enable(WDTO_8S);
}

void loop() {
  char tempF[5], humiF[5];

  dtostrf(sht25.GetTemperatureC(),5,2,tempF);
  dtostrf(sht25.GetHumidity(),5,2,humiF);

  char data[80];
  sprintf(data, "{\"deviceID\": \"%s\", \"temperature\": \"%.5s\", \"humidity\": \"%.5s\"}", deviceID, tempF, humiF);
  Serial.println(data);
  
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (client.connected()) {
    sendData(data); 
    
  } else {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    
    connectToServer();
  }
  
  // Reset watchdog
  wdt_reset();
  wdt_disable();
  
  delay(10000);
}

void connectToServer()
{
  if (client.connect(server, port)) {
    Serial.println(F("Server connected"));
  } else {
    Serial.println(F("Server connection failed")); 
  }
  delay(1000);
}

void sendData(String thisData) {
  // if there's a successful connection:
    Serial.println(F("Data sending"));
    // send the HTTP POST request:
    client.println(F("POST /api/sensors HTTP/1.1"));
    client.println(F("Host: 128.199.246.241:49157"));
    client.print(F("Content-Length: "));
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println(F("Content-Type: application/json"));
    client.println();

    // here's the actual content of the PUT request:
    client.println(thisData);
    Serial.println(F("Data sent"));
}
