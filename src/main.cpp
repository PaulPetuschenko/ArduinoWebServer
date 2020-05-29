#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xD6, 0xDA};

EthernetServer server(80);

File webFile;

int LED = 2;

void setup() {

  // serial begin
  Serial.begin(9600);
  while (!Serial)
  {
    ;
  } 

  // arduino pins init
  pinMode(LED,OUTPUT);

  // server begin
  Ethernet.begin(mac);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  //SD-card init
  if (!SD.begin())
  {
    Serial.println("Error - SD initialization failed!");
    return;
  }
  
  if (!SD.exists("index.htm"))
  {
    Serial.println("Error - there is no such file or directory!");
    return;
  }

  Serial.println("Success");

}

void loop() {
  EthernetClient client = server.available();

  if (client) 
  {
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        if (c == '\n' && currentLineIsBlank)
          {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          webFile = SD.open("index.htm");

          if (webFile) {
            while (webFile.available())
            {
              client.write(webFile.read());
            }
            webFile.close();
            
          }
          else
          {
            Serial.println("Error while loading index.htm");
          }
          

          break;
        }

        if (c == '\n')
        {
          currentLineIsBlank = true;
        }
        
        if (c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
      
    }
    delay(1);
    client.stop();
  } 
}