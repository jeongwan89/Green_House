#include <Arduino.h>
#include <TM1637Display.h>

#define CLK_1 2
#define DIO_1 3
#define CLK_2 4
#define DIO_2 5
#define CLK_3 6
#define DIO_3 7
#define CLK_4 8
#define DIO_4 9
#define CLK_5 10
#define DIO_5 11
#define CLK_6 12
#define DIO_6 13
#define CLK_7 14
#define DIO_7 15
#define CLK_8 16
#define DIO_8 17

#define TEST_DELAY

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display1(CLK_1, DIO_1);
TM1637Display display2(CLK_2, DIO_2);
TM1637Display display3(CLK_3, DIO_3);
TM1637Display display4(CLK_4, DIO_4);
TM1637Display display5(CLK_5, DIO_5);
TM1637Display display6(CLK_6, DIO_6);
TM1637Display display7(CLK_7, DIO_7);
TM1637Display display8(CLK_8, DIO_8);
int count=0;

void setup()
{
    display1.setBrightness(0x07);
    display2.setBrightness(0x07);
    display3.setBrightness(0x07);
    display4.setBrightness(0x07);
    display5.setBrightness(0x07);
    display6.setBrightness(0x07);
    display7.setBrightness(0x07);
    display8.setBrightness(0x07);

    display1.showNumberDec(0, false);
    display2.showNumberDec(0, false);
    display3.showNumberDec(0, false);
    display4.showNumberDec(0, false);
    display5.showNumberDec(0, false);
    display6.showNumberDec(0, false);
    display7.showNumberDec(0, false);
    display8.showNumberDec(0, false);

}

void loop()
{
    if(!(count % 100)) {
        display1.showNumberDec(1, false);
        display2.showNumberDec(2, false);
        display3.showNumberDec(3, false);
        display4.showNumberDec(4, false);
        display5.showNumberDec(1, false);
        display6.showNumberDec(2, false);
        display7.showNumberDec(3, false);
        display8.showNumberDec(4, false);
    }
    count++;
}