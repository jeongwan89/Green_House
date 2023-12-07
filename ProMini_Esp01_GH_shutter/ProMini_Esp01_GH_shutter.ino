#include "header.h"

// 루프 밖에서 정의 부분
// W1 (1중 서편), W2 (2중 동편), E1(1중 동편), E2(2중 동편)
Shutter W1(2, 3, 0x40), W2(4, 5, 0x41), E1(6, 7, 0x43), E2(8, 9, 0x44);

void setup(void)
{
    pinMode(LED_BUILTIN, OUTPUT); 
}

void loop(void)
{

}