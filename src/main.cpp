#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xD6, 0xDA};
IPAddress ip(192, 168, 1, 2);
EthernetServer server(80);
File  webFile;
char buffer[64];
String HTTP_req;

void setup() {

  Serial.begin(9600);
  while (!Serial)
  {
    ;
  } 
  Serial.println("Ethernet WebServer Example");

  pinMode(3, INPUT);
  pinMode(5, OUTPUT);
  Ethernet.begin(mac, ip);

  server.begin();

  // SD-card init
  if (!SD.begin(4))
  {

    return;
  }
  
  Serial.println("Success");
}

void GetDigitalSwitchState(EthernetClient cl, int pin) {
  if (digitalRead(pin))
  {
    cl.println("Switch state: ON");
  }
  else
  {
    cl.println("Switch state: OFF");
  }
}

void SetDigitalOutput(EthernetClient cl, int pin, int vol) {
  
  digitalWrite(pin, vol);
}


void loop() {
  EthernetClient client = server.available();

  if (client) 
  {
    Serial.println("new client");

    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        if ( HTTP_req.length() < 80)
          HTTP_req += c;
        
        

        if (c == '\n' && currentLineIsBlank)
          {
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          
          if (HTTP_req.indexOf("ajax_switch") > -1) {
            GetDigitalSwitchState(client, 3);
          }

          else if (HTTP_req.indexOf("enable_led") > -1) 
            SetDigitalOutput(client, 5, HIGH);
          
          else if (HTTP_req.indexOf("disable_led") > -1)
            SetDigitalOutput(client, 5, LOW);
          

          else
          {
            webFile = SD.open("index.htm");


            if(webFile) {
               

              while (webFile.available())
              {
               
                client.write(webFile.read( ) );

              
              }
              
              webFile.close();
            }
          }
          
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
      
    }
    delay(1);
    Serial.println(HTTP_req);
    HTTP_req = "";
    client.stop();
    Serial.println("client disconnected");
  } 
}

