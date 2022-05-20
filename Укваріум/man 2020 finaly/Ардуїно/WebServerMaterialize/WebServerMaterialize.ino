
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
const char* ssid = "pdm";
const char* password = "yfilsvyfipf[bcn";

//byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,80);
IPAddress gateway(192,168,0,1);
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
  WiFi.begin(ssid, password);
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
  web+="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>";
  web+="<meta charset=\"utf-8\">";
  web+="<title>ESP 8266</title>";
  web+="<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css\">";
  web+="</head>";

  web+="<body>";
  web+="<div class=\"row\">" ;
  web+=  "<div class=\"col s12\">";
  web+=   " <p  style=\"text-align: center\">----RED LED 1----</p>";
  web+= " </div>";
  web+="</div>";
  web+="<div class=\"row\">";
  web+= " <div class=\"col s6\">";
  web+=  "  <a class=\"waves-effect waves-light btn\">button</a>";
   web+= "</div>";
   web+= "<div class=\"col s6\">";
   web+=  " <a class=\"waves-effect waves-light btn\">button</a>";
  web+= " </div>";
  web+="</div>";
  web+="<body>";
  
  return(web);
}
