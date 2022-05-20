/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com  
*********/
#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"

const char* ssid = "dlink";
const char* password = "1111111111";

String web,header,;
WiFiServer server(80);

IPAddress staticIP(192,168,0,22);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

bool client_flag = true;

unsigned long int feed_time = 3000;
unsigned long int current = millis();

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  WiFi.config(staticIP, gateway, subnet);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    
    Serial.print(".");
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
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
  }
  Serial.println(web);
}
 
void loop() {
  
  WiFiClient client = server.available(); // прослушка входящих клиентов
  if (client&&client_flag) { 
    client_flag = false;
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // выводим сообщение
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime ) {
      //currentTime = millis();
      if (client.available()) { // если от клиента поступают данные,
        char c = client.read(); // читаем байт, затем
        Serial.write(c); // выводим на экран
        header += c;
        
        if (c == '\n') { // если байт является переводом строки
          // если пустая строка, мы получили два символа перевода строки
          // значит это конец HTTP-запроса, формируем ответ сервера:
          if (currentLine.length() == 0) {
            Serial.println(header);
            if(header.indexOf("GET /feed_fish")>=0){
              digitalWrite(2, HIGH);
              current = millis();
              Serial.println("Включення двигуна");
            }
            // HTTP заголовки начинаются с кода ответа (напр., HTTP / 1.1 200 OK)
            // и content-type, затем пустая строка:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println(web);
            client.println("\n\r");
            delay(5);
            break;
          }
          else { // если получили новую строку, очищаем currentLine
            currentLine = "";
          }
       }
       else if (c != '\r') { // Если получили что-то ещё кроме возврата строки,
          currentLine += c; // добавляем в конец currentLine
       }   
     }

    }
    header = "";
    
    client.stop();
    delay(1000);
    Serial.println("Client disconnected.");
    Serial.println("");
  } 

  if (millis()-current>=feed_time){
    Serial.println("Виключення двигуна");
    digitalWrite(2, LOW);
    client_flag=true;
    current = millis();
  }
}  
//web+=" <div class=\"row\"><h5>Temperature "+temp+"</h5>  </div></div></body></html>";
