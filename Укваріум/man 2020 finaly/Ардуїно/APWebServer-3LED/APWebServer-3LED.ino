
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SimpleDHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
MDNSResponder mdns;

//LCD
LiquidCrystal_I2C lcd(0x3F,16,2); // Check I2C address of LCD, normally 0x27 or 0x3F
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};  // example sprite bitmap

// Wi-Fi
const char* ssid = "dlink";
const char* password = "1111111111";

//byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,0,68);
IPAddress gateway(192,168,0,1); // router
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

//LED pins
int D0_pin = D6;
int D2_pin = D7;
int D1_pin = D8;


//Timer vars
  uint32_t ms, ms1 = 0;

//Temperature and humidity vars
byte temperature = 0;
byte humidity = 0;

//Temperature and humidity pin DHT11
int pinDHT11 = D5;

SimpleDHT11 dht11;

int err = SimpleDHTErrSuccess;



void setup(void){
  // preparing GPIOs
  pinMode(D0_pin, OUTPUT);
  digitalWrite(D0_pin, HIGH);
  pinMode(D2_pin, OUTPUT);
  digitalWrite(D2_pin, HIGH);
  pinMode(D1_pin, OUTPUT);
  digitalWrite(D1_pin, HIGH);
  //LCD
  Wire.begin(4,5);
  lcd.begin(4,5);                   
  lcd.backlight();
  lcd.createChar(1, heart);

  delay(100);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  WiFi.config(ip, gateway, subnet);
  //WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(ip, gateway, subnet);
  
 
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  //+++++++++++++++++++++++ START  LED-1 ++++++++++++++++++++
  server.on("/", [](){
    server.send(200, "text/html", webPage());
  });
  server.on("/socket1On", [](){
    digitalWrite(D0_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);
    
  });
  server.on("/socket1Off", [](){
    digitalWrite(D0_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
   //+++++++++++++++++++++++ END  LED-1 ++++++++++++++++++++ 
    
   //+++++++++++++++++++++++ START  LED-2  ++++++++++++++++++++ 
  });
  server.on("/socket2On", [](){
    digitalWrite(D2_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);    
  });
  server.on("/socket2Off", [](){
    digitalWrite(D2_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
    
   // +++++++++++++++++++++++ END  LED-2 ++++++++++++++++++++
   
   //+++++++++++++++++++++++ START  LED-3  ++++++++++++++++++++ 
  });
  server.on("/socket3On", [](){
    digitalWrite(D1_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);    
  });
  server.on("/socket3Off", [](){
    digitalWrite(D1_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
    
   // +++++++++++++++++++++++ END  LED-3 ++++++++++++++++++++
  });

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient(); 
  ms = millis();
  if( ( ms - ms1 ) > 1000 || ms < ms1 ){
      ms1 = ms;
      dht11.read(pinDHT11, &temperature, &humidity, NULL);
      lcd.setCursor(0, 0);    
      lcd.print("Temperature: ");  
      lcd.print((int)temperature);
      lcd.print("C");  
      lcd.setCursor(0, 1); 
      lcd.print("Humidity:    ");
      lcd.print((int)humidity);
      lcd.print("%");     
   }
   yield();
} 

String webPage()
{
  dht11.read(pinDHT11, &temperature, &humidity, NULL);
  String web; 
  web += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/> <meta charset=\"utf-8\"><title>ESP 8266</title><style>button{color:red;padding: 10px 27px;}</style></head>";
  web += "<h1 style=\"text-align: center;font-family: Open sans;font-weight: 100;font-size: 20px;\">ESP8266 Web Server</h1><div>";
  //++++++++++ LED-1  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----RED LED 1----</p>";
  if (digitalRead(D0_pin) == 0)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-1 +++++++++++++
  
  //++++++++++ LED-2  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----BLUE LED 2----</p>";
  if (digitalRead(D2_pin) == 0)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-2 +++++++++++++
  
  //++++++++++ LED-3  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----GREEN LED 3----</p>";
  if (digitalRead(D1_pin) == 0)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket3On\"><button>ON</button></a>&nbsp;<a href=\"socket3Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-3 +++++++++++++
  

  //Temperature
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">Temperature</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #C0C0C0;margin: 0 auto;\">T="+ String((int)temperature)+"*C</div>";
  //Temperature

  //Humidity
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">Humidity</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #C0C0C0;margin: 0 auto;\">H="+ String((int)humidity)+"%</div>";
  //Humidity
  // ========REFRESH=============
  web += "<div style=\"text-align:center;margin-top: 20px;\"><a href=\"/\"><button style=\"width:158px;\">REFRESH</button></a></div>";
  // ========REFRESH=============
  web += "</div>";
  return(web);
}
