#include <avr/wdt.h>
#include <stdlib.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LibHumidity.h>

LibHumidity sht25 = LibHumidity(0);

byte mac[] = {  
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
  
EthernetClient client;

char serverName[] = "128.199.246.241";
unsigned static int serverPort = 49156;

void setup() {
  Serial.begin(115200);
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for(;;) {
      ;
    }
  }
  
  Serial.println("Connected to Router");
  
  // Enable watchdog
  wdt_enable(WDTO_8S);
}

void loop() {
  char tempF[5], humiF[5];

  dtostrf(29.27,5,2,tempF);
  dtostrf(81.29,5,2,humiF);

  char data[255];
  sprintf(data, "{\"temperature\": \"%.5s\", \"humidity\": \"%.5s\"}", tempF, humiF);
  //Serial.println(F(data));
  
  int length = 45;
  //length = data.length();
  
  postDataToServer(length);
  
  // Reset watchdog
  wdt_reset();
  wdt_disable();
  
  delay(1000);
}

void postDataToServer(unsigned int length) {
  Serial.println(F("Connecting to server"));
  if (client.connect(serverName, 49156)) {
    Serial.println(F("Sending data"));
    client.println(F("POST /api/sensors HTTP/1.1"));
    client.println(F("HOST: 128.199.246.241:49156"));
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: 45"));
//    client.println(length);
    client.println(F("Connection: close"));
    Serial.println(F("Done"));
  } 
}
