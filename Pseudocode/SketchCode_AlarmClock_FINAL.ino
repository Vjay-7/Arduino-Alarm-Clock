
#include <DS3231.h>//RTC3231 Library
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal.h> //Libraries
#include <EEPROM.h>

// Initialization of Liquid Crystal Display
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Arduino pins to lcd

#define bt_time   A1
#define bt_up     A2
#define bt_alarm  A3

#define buzzer 8

// Initialization of DS3231
DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t;

int hour = 0, minute = 0, sec = 0, date = 0, mon = 0, set_day;
int year = 0;
String Day = "  ";

int AlarmHH  = 23, AlarmMM  = 59, AlarmSS  = 59, setMode = 0, setAlarm = 0, alarmMode=0;

int stop =0, mode=0, flag=0, gdsc= 0;

//Eeprom Store Variable
uint8_t HH;
uint8_t MM;

 byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B01110,
        B00100};
byte thermometer_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};


void setup(){
    // Setup Serial connection
      Serial.begin(9600);

      rtc.begin(); 

    pinMode(bt_time,  INPUT_PULLUP);
    pinMode(bt_up,    INPUT_PULLUP);
    pinMode(bt_alarm, INPUT_PULLUP);
    pinMode(9, OUTPUT);
    pinMode(buzzer, OUTPUT);

      lcd.createChar(1, thermometer_symbol);
      lcd.createChar(2, bell_symbol);
      
      lcd.begin(16, 2); 
      lcd.setCursor(0,0);  //Show "TIME" on the LCD


    stop=EEPROM.read(50);
    if(stop==0){  
    }else{WriteEeprom ();}

    EEPROM.write(50,0); 

    ReadEeprom();
    // Setting date and time through code
    //rtc.setDOW(2);     // Set Day-of-Week to SUNDAY
    //rtc.setTime (00, 9, 50); 
    //rtc.setDate(12, 11, 2017);  
}

void loop(){  
        t = rtc.getTime();
        Day = rtc.getDOWStr(1);

        if (setMode == 0){
            hour = t.hour,DEC;
            minute = t.min,DEC;
            sec = t.sec,DEC;
            mon = t.mon,DEC;
            date = t.date,DEC;
            year = t.year,DEC;
        }  

          if(setAlarm==0){
                lcd.setCursor(0,0); 
                lcd.print((hour/10)%10);
                lcd.print(hour % 10); 
                lcd.print(":");
                lcd.print((minute/10)%10);
                lcd.print(minute % 10);
                lcd.print(":");
                lcd.print((sec/10)%10);
                lcd.print(sec % 10);
                lcd.print(" ");  
                if(mode==1){lcd.write(2);}
                else{lcd.print(" ");}   
                lcd.print(" "); 
                lcd.write(1); 
                lcd.print(rtc.getTemp(),0);
                lcd.write(223); 
                lcd.print("C");
                lcd.print("  "); 

                lcd.setCursor(1,1);
                lcd.print(Day);
                lcd.print(" ");
                lcd.print((mon/10)%10);
                lcd.print(mon % 10);
                lcd.print("/");
                lcd.print((date/10)%10);
                lcd.print(date % 10); 
                lcd.print("/"); 
                lcd.print((year/1000)%10);
                lcd.print((year/100)%10);
                lcd.print((year/10)%10);
                lcd.print(year % 10);
          }
        /*
          if (digitalRead(bt_up) == 0){
            while(true){
              lcd.setCursor(0,0);
              lcd.print("~ GDSC BOOTH ~");
              if(digitalRead(bt_up)==0) break;
            }
          }
          */

                setupClock();
                setTimer();
                delay (100);
                blinking();
                if (alarmMode==1 && mode==1 && hour==AlarmHH && minute==AlarmMM && sec==AlarmSS) {
                    while (true){ 
                    digitalWrite(buzzer, HIGH);
                    if (alarm() ) break;
                  }
                    digitalWrite(buzzer, LOW);
                  }
                  else{digitalWrite(buzzer, LOW);}

        delay (100);
}


 int alarm(){
   if (digitalRead(bt_alarm)==0) return 1;
   else return 0;
 }




void blinking (){
    //BLINKING SCREEN
    if (setAlarm <2 && setMode == 1){lcd.setCursor(0,0);  lcd.print("  ");}
    if (setAlarm <2 && setMode == 2){lcd.setCursor(3,0);  lcd.print("  ");}
    if (setAlarm <2 && setMode == 3){lcd.setCursor(6,0);  lcd.print("  ");}
    if (setAlarm <2 && setMode == 4){lcd.setCursor(1,1);  lcd.print("   ");}
    if (setAlarm <2 && setMode == 5){lcd.setCursor(5,1);  lcd.print("  ");}
    if (setAlarm <2 && setMode == 6){lcd.setCursor(8,1);  lcd.print("  ");}
    if (setAlarm <2 && setMode == 7){lcd.setCursor(11,1); lcd.print("    "); }
    //Alarm
    if (setMode == 0 && setAlarm == 1){lcd.setCursor(6,0); lcd.print("           "); }
    if (setMode == 0 && setAlarm == 2){lcd.setCursor(4,1); lcd.print("  "); }
    if (setMode == 0 && setAlarm == 3){lcd.setCursor(7,1); lcd.print("  "); }
    if (setMode == 0 && setAlarm == 4){lcd.setCursor(10,1);lcd.print("  "); }
}

// Set-up for Clock customizing
void setupClock (void) {
        if (setMode == 8){
        lcd.setCursor (0,0);
        lcd.print (F("Set Date Finish "));
        lcd.setCursor (0,1);
        lcd.print (F("Set Time Finish "));
        delay (1000);
        rtc.setTime (hour, minute, sec);
        rtc.setDate (mon, date, year);  
        lcd.clear();
        setMode = 0;
        }

        if (setAlarm == 5){
        lcd.setCursor (0,0);
        lcd.print (F("Set Alarm Finish"));
        lcd.setCursor (0,1);
        lcd.print (F(" Prepare to Wake-Up"));
        WriteEeprom();
        delay (2000); 
        lcd.clear();
        setAlarm=0;
        alarmMode=1;
        }
        
    if (setAlarm >0){ alarmMode=0;}
        
    if(digitalRead (bt_time) == 0 && flag==0) {flag=1;
    if(setAlarm>0){setAlarm=5;}   // Jump to terminate the time setup
    else{setMode = setMode+1;}
    }
      
    if(digitalRead (bt_alarm) == 0 && flag==0){flag=1;
    if(setMode>0){setMode=8;}     //Jump to terminate the alarm setup
      else{setAlarm = setAlarm+1;} 
      lcd.clear();} 

    if(digitalRead (bt_time) == 1 && digitalRead (bt_alarm) == 1){flag=0;}
      
    if(digitalRead (bt_up) == 0){                          
     if (setAlarm<2 && setMode==1)hour=hour+1; 
     if (setAlarm<2 && setMode==2)minute=minute+1;
     if (setAlarm<2 && setMode==3)sec=sec+1;
                if (setAlarm<2 && setMode==4)set_day=set_day+1;
                if (setAlarm<2 && setMode==6)date=date+1;
                if (setAlarm<2 && setMode==5)mon=mon+1;
                if (setAlarm<2 && setMode==7)year=year+1;
                //Alarm
                if (setMode==0 && setAlarm==1)mode +=1;
                if (setMode==0 && setAlarm==2 && AlarmHH<23)AlarmHH=AlarmHH+1;
                if (setMode==0 && setAlarm==3 && AlarmMM<59)AlarmMM=AlarmMM+1;
                if (setMode==0 && setAlarm==4 && AlarmSS<59)AlarmSS=AlarmSS+1;

                if(hour>23)hour=0;
                if(AlarmHH>=24)AlarmHH=0;
                if(minute>59)minute=0;
                if (AlarmMM>=60) AlarmMM=0;
                if(sec>59)sec=0;
                if (AlarmSS>=60) AlarmSS=0;
                if(set_day>7)set_day=0;
                if(date>31)date=0;
                if(mon>12)mon=0;
                if(year>2030)year=2000;
                if (mode>1) mode=0;
                rtc.setDOW(set_day);
    }       
}

void setTimer (){
  //Alarm
 if (setMode == 0 && setAlarm >0){
    lcd.setCursor (0,0);
    lcd.print("Alarm ");
    if(mode==0){
      lcd.print("OFF");
      digitalWrite(9, LOW);
    }
    else{
      lcd.print("ON");
      digitalWrite(9, HIGH);
    }
      
          
    lcd.setCursor (4,1);
    lcd.print((AlarmHH/10)%10);
    lcd.print(AlarmHH % 10);
    lcd.print(":");
    lcd.print((AlarmMM/10)%10);
    lcd.print(AlarmMM % 10);
    lcd.print(":");
    lcd.print((AlarmSS/10)%10);
    lcd.print(AlarmSS % 10);
 }
}

void ReadEeprom () {
  AlarmHH=EEPROM.read(1);
  AlarmMM=EEPROM.read(2);
  AlarmSS=EEPROM.read(3);
  
  mode=EEPROM.read(4); 
}

void WriteEeprom () {
  EEPROM.write(1,AlarmHH);
  EEPROM.write(2,AlarmMM);
  EEPROM.write(3,AlarmSS);
  EEPROM.write(4,mode);
}


