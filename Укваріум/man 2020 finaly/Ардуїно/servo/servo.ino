#include <Servo.h> //используем библиотеку для работы с сервоприводом

Servo servo; //объявляем переменную servo типа Servo


unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;


void setup() //процедура setup

{
Serial.begin(9600);
servo.attach(D10); //привязываем привод к порту 10
servo.write(0); //ставим вал под 0
}

void loop() //процедура loop

 {servo.write(0); //ставим вал под 0
  unsigned long currentMillis = millis();
  Serial.println(currentMillis);
  if (currentMillis - previousMillis >= interval) {
   
    previousMillis = currentMillis;
    
  }
 }
