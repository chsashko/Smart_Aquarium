#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#include <Servo.h>
Servo servo1;

const long interval = 1000;
unsigned long previousMillis = 0;  

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  servo1.attach(D10);
  lcd.init();                     
  lcd.backlight();
  Serial.begin(9600);
  pinMode(D2,OUTPUT);
  analogWrite(D2,0);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
}

void loop() {
  tmElements_t tm;
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();


 
 int s = 40;
 int r = 10;

 String a="Nich";
 checktime();
 
 if (tm.Second>=r && tm.Second<=14 )
     {
    for (int i=0; i<255; i=i+1)
 {  String a="Ranok";
 checktime();
 analogWrite(D2,i);
 delay(5); 
 Serial.println(i);

  }
  }
 
if (tm.Second>=15 && tm.Second<=34 )
  { int i=1023;
    String a="Den'";
    checktime();
    analogWrite(D2,i);

    }
     
  if (tm.Second>=34 && tm.Second<=36)
{
  for (int i=1023; i>=0; i=i-1)
 {
  String a="Vechir";
  checktime();
  analogWrite(D2,i);
  delay(5); 
  Serial.println(i);
  }
  }   

  if (tm.Second>=37)
 { String a="Nich";
  checktime();
  analogWrite(D2,0);
  }

    
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}
servo()
void checktime()
{ 
 String a="";
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print(a);
 lcd.setCursor(0,1); 
 tmElements_t tm;
 lcd.print( tm.Hour);
 lcd.setCursor(3,1);
 lcd.print(':');
 lcd.setCursor(5,1);
 lcd.print(tm.Minute);
  }
 
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
void servo()
{ 
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)  
    {previousMillis = currentMillis;
   servo1.write(180);
}
}
