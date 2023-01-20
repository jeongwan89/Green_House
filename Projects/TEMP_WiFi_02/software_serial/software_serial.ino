#include <LiquidCrystal_I2C.h>
 
int y,m,d,h,mm,s;
char *week[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
char *ampm[2]={"AM","PM"};
unsigned char hh, ap, day;
int yy;
char rx_buf[20], str[21];
byte rx_cnt=0;
boolean rx_flag = false;
 
LiquidCrystal_I2C lcd(0x27,16,2);
//================================================================
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}
//================================================================
void loop() {
  if (rx_flag) {
    if(h>12) hh=h-12;
    else if(h==0) hh=12;
    else hh=h;
    if(h<=11) ap=0; else ap=1;
    yy=2000+y;
    day=(char)day_of_week(yy,m,d);
    sprintf(str,"20%02d-%02d-%02d [%s]", y,m,d,week[day]);
    lcd.setCursor(0, 0); lcd.print(str); Serial.print(str); Serial.print(" ");
    sprintf(str,"[%s] %02d:%02d:%02d   ", ampm[ap],hh,mm,s);
    lcd.setCursor(0, 1); lcd.print(str); Serial.println(str);
    rx_flag = false;
  }
}
//================================================================
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if((inChar=='#')||(inChar==2)) {rx_cnt=0;}
    else if((inChar=='$')||(inChar=='%')||(inChar==3)){
      if(rx_cnt==12){
        for(int i=0;i<12;i++) rx_buf[i]&=0x0F;
        y =rx_buf[ 0]*10 + rx_buf[ 1];
        m =rx_buf[ 2]*10 + rx_buf[ 3];
        d =rx_buf[ 4]*10 + rx_buf[ 5];
        h =rx_buf[ 6]*10 + rx_buf[ 7];
        mm=rx_buf[ 8]*10 + rx_buf[ 9];
        s =rx_buf[10]*10 + rx_buf[11];
        rx_flag = true; 
      }
      rx_cnt=0;
    }
    else {
      if(rx_cnt<16) rx_buf[rx_cnt++]= inChar;
    }
  }
}
int day_of_week(int y, int m, int d){
    unsigned char t[]={0,3,2,5,0,3,5,1,4,6,2,4};
    y-= m<3;
    return (y+y/4-y/100+y/400+t[m-1]+d) %7;
}
//================================================================
