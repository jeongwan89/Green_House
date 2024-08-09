#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define debug
#ifdef  debug
#define PWM_LED 5
#endif

// Pin 정의
#define VR  A0
#define VR_DURA A1
#define PWM 3
#define DIR 4
#define DIR_SW  6
#define FEED 7
#define ONE_STEP 8
#define DIR_LED 9
#define MODE_LED 10
#define MODE_SELECT_SW 11
// 상주 정의
#define ONE_STEP_DURATION 1000
#define MAX_SPEED 255   // UP TO 255
#define DURA_MIN 1 // mode1에서 최소 지속시간
#define DURA_MAX 30 // mode1에서 최대 지속시간
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
/*     for(int i=0; i < 256; i++)
    {
        if(currVrMapped == 0) break;
        currVrMapped -- ;
        analogWrite(PWM, currVrMapped);
        #ifdef debug
        analogWrite(PWM_LED, currVrMapped);
        #endif
        delay(20);
    } */
    motorDir = !motorDir;
    digitalWrite(DIR, motorDir);
    digitalWrite(DIR_LED, motorDir);
    // increment
/*     for(int i=0; i < 256; i++)
    {
        if(currVrMapped == initialPWM) break;
        currVrMapped ++ ;
        analogWrite(PWM, currVrMapped);
        #ifdef debug
        analogWrite(PWM_LED, currVrMapped);
        #endif
        delay(40);
    } */
}

/**
 * 프로그램을 짜다가 보니 한번 누를 때마다 일정 거리를 갈 수 있는 장치가 있어야 함
 * 스위치를 하나 달고(mode라고 이름 부친다) 방향전환과 이 스위치(mode)가 같이 눌러지면
 * 모드를 바꾼다. 전역을 선언된 bool mode를 토글하고 동시에 이 값을 리턴한다.
 */
int modeCheck(void)
{
    static int lastMode=1;
    static int flip = 0; 
    int currMode;
    currMode = digitalRead(MODE_SELECT_SW);
    delay(100);
    if(lastMode == 1 && currMode == 0) 
    {
        mode = !mode;
        // 두 조건으로 mode가 바뀌었음으로 그 전의 조건들을 초기화 한다.
        // 문제가 되는 핀은 PWM와 디버그 확인용 PWM_LED 이다.
        analogWrite(PWM, 0);
        #ifdef debug
        analogWrite(PWM_LED, 0);
        Serial.println("Now mode changed");
        #endif
        digitalWrite(MODE_LED, mode);
    }
    lastMode = currMode;
    return mode;
}

/**
 * mode 1에서 필요한 함수.
 * @param: duration은 가속 지속 시간을 의미한다.
 */
void moveOneStep(unsigned long duration, int currSpeed)
{
    // 가속구간
    for(int i = 0; i < currSpeed; i++)
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
    for(int i = currSpeed; i >= 0; i--)
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
    pinMode(DIR_LED, OUTPUT);
    pinMode(MODE_LED, OUTPUT);
    pinMode(DIR_SW, INPUT_PULLUP);
    pinMode(FEED, INPUT_PULLUP);
    pinMode(ONE_STEP, INPUT_PULLUP);
    pinMode(MODE_SELECT_SW, INPUT_PULLUP);
    pinMode(VR_DURA, INPUT);

#ifdef debug
    pinMode(PWM_LED, OUTPUT);
    Serial.begin(115200);
#endif
    digitalWrite(DIR, LOW);
}

void loop()
{
    static int lastStateSW = 1;
    static int lastMode = 0;
    static int keepDir = 0;
    static int lastOneStep = 1;
    int stateSW = 1; // 방향
    int modeSW = 1;  // 모드
    int currMode;
    int currOneStep;
    int vrValue;
    int vrMapped;
    int duraMode1;
    int duraMode1Mapped;
    // mode 0 : 연속적으로 움직이는 상태. 방향전환 키를 누르면 회전을 역전
    // mode 1 : mode를 한번 누르면 일정한 시간만큼 가는 상태. 방향 전환키는 회전을 역전
    currMode = modeCheck();
    vrValue = analogRead(VR);
    vrMapped = map(vrValue, 7, 1023, 0, MAX_SPEED);
    vrMapped > 0 ? : vrMapped = 0;

    if(currMode == 0) {
        // mode 0 :
        motorDir = keepDir;

    #ifdef debug
        Serial.print("vrValue : ");
        Serial.println(vrValue);
        analogWrite(PWM_LED, vrMapped);
    #endif
        analogWrite(PWM, vrMapped);
        stateSW = digitalRead(DIR_SW);
        if(lastStateSW ==1 && stateSW ==0)
        {
            changeDir(vrMapped);
        }
        keepDir = motorDir;
    } 
    // 조건부에 들어가면서 mode 1:이 처음으로 행해지는 시기의 조건을 정의한다.
    // 건들여졌던 motorDir을 연속적으로 사용하여야 하겠다.
    else // mode 1 : 계속
    {
        // MODE(define에 정의됨)가 눌러지면(LOW) 일정기간 움직임. 
        // 방향은 DIR(define에 정의됨)에 따라 움직인다. mode 0에서 정의됨 값을
        // 그대로 사용한다.
        stateSW = digitalRead(DIR_SW);
        duraMode1 = analogRead(VR_DURA);
        duraMode1Mapped = map(duraMode1, 10, 1023, DURA_MIN, DURA_MAX);
        duraMode1Mapped > DURA_MIN ? : duraMode1Mapped = DURA_MIN;
        #ifdef debug
        Serial.print("duraMode1 : ");
        Serial.println(duraMode1);
        #endif
        if(lastStateSW == 1 && stateSW == 0){
            motorDir = !motorDir;
            digitalWrite(DIR, motorDir);
            digitalWrite(DIR_LED, motorDir);
        }
        currOneStep = digitalRead(ONE_STEP);
        delay(100);
        if(lastOneStep == 1 && currOneStep == 0)
        {
            moveOneStep(duraMode1Mapped * 1000, vrMapped);
        }
        lastStateSW = stateSW;
        keepDir = motorDir;
        lastOneStep = currOneStep;
    }
    lastMode = currMode;
    lastStateSW = stateSW;
}