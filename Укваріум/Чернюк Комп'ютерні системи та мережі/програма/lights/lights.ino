#include "RTClib.h"

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String _time = "";

void setup() {
 Serial.begin(9600);
pinMode(D2, OUTPUT);//на світодіод

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }
}
void loop() {
  // put your main code here, to run repeatedly:

    Serial.println(_time);
    DateTime now = rtc.now();

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

 int s = 40;
 int r = 10;




 
 if(now.second()>=r && now.second()<=r+4 )
 { for (int i=0; i<1024; i=i+1)
 {
    _time="Morning";
    analogWrite(D2,i);
    delay(2); 
    Serial.println(i);
 }
 }
 
if (now.second()>=r+5 && now.second()<=s-1 )
  { int i=1023;
    _time="Day";
    analogWrite(D2,i);

    }
     
  if (now.second()>=s && now.second()<=s+4)
{
  for (int i=1023; i>=0; i=i-1)
 {
  _time="Evening";
  analogWrite(D2,i);
  delay(2); 
  Serial.println(i);
  }
  }   

  if (now.second()>=s+3)
 { _time="Night";
  analogWrite(D2,0);
  }
}
