#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Arduino.h>
#include "MillisTimer.h"
#include "RTClib.h"
#include <Wire.h>
#include "timeParser.h"
#include "js.h" 

//for d18b20
#define ONE_WIRE_BUS 14
 //settings for timer
#define I2C_SDA 12
#define I2C_SCL 13
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Servo servo;
String web,header;

// Config WiFi parameters
const char* ssid = "TP-Link_AE9F";
const char* password = "00031950";

IPAddress staticIP(192,168,43,122);
IPAddress gateway(192,168,43,1);
IPAddress subnet(255,255,255,0);

// WebServer create 
AsyncWebServer server(80);

//Time config
unsigned long int feed_time = 3000;
MillisTimer feed_timer = MillisTimer(feed_time);

// Init FORM DATA parameters
const char* PARAM_INPUT_1 = "vkl";
const char* PARAM_INPUT_2 = "vukl";
const char* PARAM_INPUT_3 = "zzz";

String time_vkl; // "08:00"
String time_vukl;
String time_zzz;
MillisTimer time_timer = MillisTimer(59999);

bool motor_run = false;
bool anti_double_feed  = false;
MillisTimer timer_anti_double_feed = MillisTimer(43200000);

bool light_status = false;
int light_0_256 = 0;
int temp = 0;
MillisTimer light_timer = MillisTimer(1000);

//D18B20 temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
MillisTimer temperature_timer = MillisTimer(feed_time);


void turn_off_motor(MillisTimer &mt){
  servo.write(90);
  motor_run = false;
  feed_timer.reset();
  Serial.println("Motor OFF");
  servo.write(90);
  }

void security_watchdog(MillisTimer &mt){
    anti_double_feed  = false;
}

void measure_temp(MillisTimer &mt){
  //dht11.read(&temp, &hum, NULL);
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println("Tempereture measurment:" + String(temp));
  mt.reset();
  mt.setInterval(feed_time);
  mt.expiredHandler(measure_temp);
  mt.setRepeats(1);
  mt.start();
}

void lightUpdate(MillisTimer &mt){
    if (light_status) {
        if (light_0_256 <255){
             light_0_256+=8;
          }
      }
    else {
        if (light_0_256 > 0){
             light_0_256-=8;
          }
    }
    Serial.println("Analog points: " + String(light_0_256));
    analogWrite(2, light_0_256); 
    mt.setRepeats(1);
    mt.start();
}


void timer_ds(MillisTimer &mt){
    DateTime now = rtc.now();
          
    //Time variables
    String vkl_hour, vkl_minute = "";   // "08", "00"
    String vukl_hour, vukl_minute = "";
    String zzz_hour, zzz_minute = "";

    zzz_hour = getValue(time_zzz, ':', 0);
    zzz_minute = getValue(time_zzz, ':', 1);
    vkl_hour = getValue(time_vkl, ':', 0);
    vkl_minute = getValue(time_vkl, ':', 1);
    vukl_hour = getValue(time_vukl, ':', 0);
    vukl_minute = getValue(time_vukl, ':', 1);
    if (zzz_hour.charAt(0)=='0'){zzz_hour = zzz_hour.charAt(1);}
    if (zzz_minute.charAt(0)=='0'){zzz_minute = zzz_minute.charAt(1);}
    if (vkl_hour.charAt(0)=='0'){vkl_hour = vkl_hour.charAt(1);}
    if (vkl_minute.charAt(0)=='0'){vkl_minute = vkl_minute.charAt(1);}
    if (vukl_hour.charAt(0)=='0'){vukl_hour = vukl_hour.charAt(1);}
    if (vukl_minute.charAt(0)=='0'){vukl_minute = vukl_minute.charAt(1);}
    
    if ((String)now.hour()==zzz_hour && (String)now.minute()==zzz_minute){
        Serial.println(zzz_hour+" "+zzz_minute);
        Serial.println("flag");
        motor_run =true;
        feed_timer.expiredHandler(turn_off_motor);
        feed_timer.setInterval(feed_time);
        feed_timer.setRepeats(1);
        feed_timer.start();
    }
    Serial.print(now.hour() + vkl_hour);
    if ((String)now.hour()==vkl_hour && (String)now.minute()==vkl_minute){
      light_status = true;
      }

    if ((String)now.hour()==vukl_hour && (String)now.minute()==vukl_minute){
      light_status = false;
      }
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    mt.setRepeats(1);
    mt.start();
}

void setup() {
  //d18b20
  sensors.begin(); 
 
   Serial.begin(115200);
   
   servo.attach(15);                   //servo
   
   pinMode(2, OUTPUT);                 //mosfet
   digitalWrite(2, LOW);

   Serial.println("Prepare pins");
  pinMode(I2C_SCL,INPUT_PULLUP);//SDA // ds1307rtc
  pinMode(I2C_SDA,INPUT_PULLUP);//SCL
   Serial.println("Prepare pins Done");
  Wire.begin(I2C_SCL,I2C_SDA);       
   Serial.println("I2C up and running"); 
   
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    abort(); }

    
   if(!WiFi.config(staticIP, gateway, subnet)){
    Serial.println("Cant configure wifi!");}

  Serial.println("Prepare WiFi");
   WiFi.begin(ssid, password);          //WiFi
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   }
   Serial.println("Connected to Wifi");
 
  
  //file system read 
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 
  File file = SPIFFS.open("/index.html", "r");
  Serial.println();
  Serial.println("File Content:");
  delay(2000);
  while (file.available()) 
  {
  web+=String((char)file.read());
  }
  file.close();
 
 
  temperature_timer.expiredHandler(measure_temp);
  temperature_timer.setInterval(feed_time);
  temperature_timer.setRepeats(1);
  temperature_timer.start();

  time_timer.expiredHandler(timer_ds);
  time_timer.setRepeats(1);
  time_timer.start();

  light_timer.expiredHandler(lightUpdate);
  light_timer.setRepeats(1);
  light_timer.start();
   // Web server events settings
   server.on("/get", HTTP_POST, [] (AsyncWebServerRequest *request) {
        String inputMessage;
        String inputParam;
        if (request->hasParam(PARAM_INPUT_1, true)) {
          inputMessage = request->getParam(PARAM_INPUT_1, true)->value();
          time_vkl=inputMessage;
          Serial.println("Morning: "+time_vkl);
          
        }
        if (request->hasParam(PARAM_INPUT_2, true)) {
          inputMessage = request->getParam(PARAM_INPUT_2, true)->value();
          time_vukl=inputMessage;
          Serial.println("Evening: "+time_vukl);
        }
        if (request->hasParam(PARAM_INPUT_3, true)) {
          inputMessage = request->getParam(PARAM_INPUT_3, true)->value();
          time_zzz=inputMessage;
          Serial.println("Nyam nyam)"+time_zzz);
        }
        String html_send = web + showTemp(temp) + feedButton(anti_double_feed);
        request->send(200, "text/html", html_send+setRozklad(time_vkl, time_vukl, time_zzz));
   });
   server.on("/feed_fish", HTTP_GET, [] (AsyncWebServerRequest *request){
      if (!anti_double_feed) {
        motor_run = true;
        anti_double_feed = true;
        feed_timer.expiredHandler(turn_off_motor);
        feed_timer.setInterval(feed_time);
        feed_timer.setRepeats(1);
        feed_timer.start();
        timer_anti_double_feed.expiredHandler(security_watchdog);
        timer_anti_double_feed.setInterval(43200000);
        timer_anti_double_feed.setRepeats(1);
        timer_anti_double_feed.start();
       }
      String html_send = web +  showTemp (temp) + setRozklad(time_vkl, time_vukl, time_zzz) + feedButton(anti_double_feed);
      request->send(200, "text/html", html_send);
   });
   server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request){
      String html_send = web +  showTemp (temp)+ feedButton(anti_double_feed);
      request->send(200, "text/html", html_send + setRozklad(time_vkl, time_vukl, time_zzz));
   });
   server.begin();
   
}

void loop() {
  motor_run = false;
  temperature_timer.run();
  feed_timer.run();
  time_timer.run();
  light_timer.run();
  timer_anti_double_feed.run();
  if (motor_run ==true){
      servo.write(0);
      Serial.println("Motor ON");
    } 
   
}
