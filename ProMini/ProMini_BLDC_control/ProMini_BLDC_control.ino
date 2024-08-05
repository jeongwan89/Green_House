#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define debug
#ifdef  debug
#define PWM_LED 5
#endif


#define VR  A0
#define PWM 3
#define DIR 4
#define DIR_SW  6
#define FEED 7
#define MODE 8
#define ONE_STEP_DURATION 1000
#define MAX_SPEED 100   // UP TO 255
bool motorDir = 0;
bool mode = 0;
/**
 * loop()에서 방향을 바꿀 때(pin DIR 사용) 부드럽게 정지하고 
 * 다시 원래 그 속도로 회복하기 위한 함수
 * @param : initialPWM  당시 회전하고 있는 속도
 */
void changeDir(int initialPWM)
{
    int currVrMapped = initialPWM;
    // decrement
    for(int i=0; i < 256; i++)
    {
        if(currVrMapped == 0) break;
        currVrMapped -- ;
        analogWrite(PWM, currVrMapped);
        #ifdef debug
        analogWrite(PWM_LED, currVrMapped);
        #endif
        delay(20);
    }
    motorDir = !motorDir;
    digitalWrite(DIR, motorDir);
    // increment
    for(int i=0; i < 256; i++)
    {
        if(currVrMapped == initialPWM) break;
        currVrMapped ++ ;
        analogWrite(PWM, currVrMapped);
        #ifdef debug
        analogWrite(PWM_LED, currVrMapped);
        #endif
        delay(40);
    }
}

/**
 * 프로그램을 짜다가 보니 한번 누를 때마다 일정 거리를 갈 수 있는 장치가 있어야 함
 * 스위치를 하나 달고(mode라고 이름 부친다) 방향전환과 이 스위치(mode)가 같이 눌러지면
 * 모드를 바꾼다. 전역을 선언된 bool mode를 토글하고 동시에 이 값을 리턴한다.
 */
int modeCheck(void)
{
    static int lastDir=1, lastMode=1;
    int currDir, currMode;
    currDir = digitalRead(DIR_SW);
    currMode = digitalRead(MODE);
    delay(100);
    if(lastDir == 0 && currDir ==  0 && lastMode == 0 && currMode == 0) 
        // 방향키가 눌러졌다
        // 모드키가 눌려졌다.
    {
        mode = !mode;
        // 두 조건으로 mode가 바뀌었음으로 그 전의 조건들을 초기화 한다.
        // 문제가 되는 핀은 PWM와 디버그 확인용 PWM_LED 이다.
        analogWrite(PWM, 0);
        #ifdef debug
        analogWrite(PWM_LED, 0);
        Serial.println("Now mode changed");
        #endif

        // 계속 누르고 있을 때가 문제!
        // 계속 누르고 있는지 확인하고 끝이 났을 때 정상으로 process진행
        while(!(digitalRead(DIR_SW) && digitalRead(MODE)))
        {
            currDir = digitalRead(DIR_SW);
            currMode = digitalRead(MODE);
            #ifdef debug
            Serial.println("Now exit modeCheck()");
            #endif
        }
    }
    lastDir = currDir;
    lastMode = currMode;
    return mode;
}

/**
 * mode 1에서 필요한 함수.
 * @param: duration은 가속 지속 시간을 의미한다.
 */
void moveOneStep(unsigned long duration)
{
    // 가속구간
    for(int i = 0; i < MAX_SPEED; i++)
    {
        analogWrite(PWM, i);
        #ifdef debug
        analogWrite(PWM_LED, i);
        #endif
        delay(10);
    }
    // 지속구간
    delay(duration);
    // 감속구간
    for(int i = MAX_SPEED; i >= 0; i--)
    {
        analogWrite(PWM, i);
        #ifdef debug
        analogWrite(PWM_LED, i);
        #endif
        delay(10);
    }
    analogWrite(PWM, 0);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(VR, INPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(DIR_SW, INPUT_PULLUP);
    pinMode(FEED, INPUT_PULLUP);
    pinMode(MODE, INPUT_PULLUP);

#ifdef debug
    pinMode(PWM_LED, OUTPUT);
    Serial.begin(115200);
#endif
    digitalWrite(DIR, LOW);
}

void loop()
{
    static int lastStateSW = 1;
    static int lastModeSW = 1;
    static int lastMode = 0;
    int stateSW = 1;
    int modeSW = 1;
    int currMode;
    int vrValue;
    int vrMapped;
    // mode 0 : 연속적으로 움직이는 상태. 방향전환 키를 누르면 회전을 역전
    // mode 1 : mode를 한번 누르면 일정한 시간만큼 가는 상태. 방향 전환키는 회전을 역전
    currMode = modeCheck();

    if(currMode == 0) {
        vrValue = analogRead(VR);
        vrMapped = map(vrValue, 7, 1023, 0, 255);
        vrMapped > 0 ? : vrMapped = 0;

    #ifdef debug
        Serial.println(vrValue);
        analogWrite(PWM_LED, vrMapped);
    #endif
        analogWrite(PWM, vrMapped);
        stateSW = digitalRead(DIR_SW);
        if(lastStateSW != stateSW)
        {
            delay(100);
            if(stateSW == LOW) 
            {
                changeDir(vrMapped);
            }
            lastStateSW = stateSW;
        }
    } 
    else // modeCheck가 0이 아니라는 것은 모드가 1로 바뀌었다는 뜻
    {
        // MODE(define에 정의됨)가 눌러지면(LOW) 일정기간 움직임. 
        // 방향은 DIR(define에 정의됨)에 따라 움직인다. mode 0에서 정의됨 값을
        // 그대로 사용한다.
        modeSW = digitalRead(MODE);
        stateSW = digitalRead(DIR_SW);
        if(lastStateSW == 0 && stateSW == 0){
            motorDir = !motorDir;
            digitalWrite(DIR, motorDir);
        }
        delay(100);
        if(lastModeSW == 0 && modeSW == 0)
        {
            moveOneStep(ONE_STEP_DURATION);
        }
        lastModeSW = modeSW;
        lastStateSW = stateSW;
    }
    lastMode = currMode;
}