#ifndef mylib_h
#define mylib_h

#include "Arduino.h"

/*
    Arduino에서 함수를 인자로 호출하는 Class작성
    이 class는 loop()에 들어가기는 하지만, 정해진 주기에 한번 이 함수를 실행한다.
*/
//====================================================================================================================

// 루프 안에서 일정 주기가 돌아오면 그 주기 시각에 어떤 일을 하는 객체
// ex. 주기 5000 millis 마다 LED를 토글한다.
// ex. 주기 10,000 millis 마다 온도 센서를 읽는다.

class RaiseTimeEventInLoop
{
public:
    // t_anchor : static이고 초기는 이 함수가 실행되는 때의 시간.
    // 이후로는 period_t가 지난 후의 시스템 시간
    unsigned long t_anchor;
    // t_current : 루프 중에 시스템 시간을 계산한다.
    unsigned long t_current;
    void (*pfunct)(void); // 생성자에서 실행할 함수를 기억할 용도로 쓸 예정이었는데, 그 효용성이 떨어졌다. 하위 버전 호환성을 위해 그냥 둔다.

public:
    RaiseTimeEventInLoop()
    {
        t_anchor = millis();
        t_current = millis();
    }
    RaiseTimeEventInLoop(void (*fptr)(void))
    { // 더이상 쓰지 말 것
        t_anchor = millis();
        t_current = millis();
        pfunct = fptr;
    }
    void eachEveryTimeIn(unsigned long t_period) { eachEveryTimeIn((t_period), pfunct); } // 더이상 쓰지 말 것
    void eachEveryTimeIn(unsigned long t_period, void (*fptr)(void));
    // 이하는 인수를 위한 over load형태임
    void eachEveryTimeIn(unsigned long t_period, void (*fptr)(const char *, const char *), const char *, const char *);
    void eachEveryTimeIn(unsigned long, void (*fptr)(const char *, int), const char *, int);
    void eachEveryTimeIn(int t_period, void (*fptr)(void)) { eachEveryTimeIn((unsigned long)t_period, fptr); }
    void eachEveryTimeIn(int t_period, void (*fptr)(const char *, const char *), const char *a, const char *b)
    {
        eachEveryTimeIn((unsigned long)t_period, fptr, a, b);
    }
    void eachEveryTimeIn(int t_period, void (*fptr)(const char *, int), const char *arg_a, int arg_b)
    {
        eachEveryTimeIn((unsigned long)t_period, fptr, arg_a, arg_b);
    }

    // void operator()();
    // void operator()(void (*fptr)(void));
};

//==============================================================================================================
// 루프에서 어떤 시간동안 어떤 동작을 하는 클래스.
// ex. 60초 주기 5초동안에 분사하는 동작
// 필요요소: 주기, 동작시간, 동작정의함수
//==============================================================================================================
class PeriodicMthdInLoop
{
    // member variable
public:
    unsigned long t_period, t_duration, t_anchor, t_current;
    void (*fptr_dr)(void);   // 실행 시간동안 행해지는 함수
    void (*fptr_dflt)(void); // 실행 시간 밖에 있는 함수. 일종의 default함수
private:
    bool prev_flg, curr_flg; // 매개 함수 실행을 딱 한번 하기 위한 상태값
    // member function
public:
    // constructor
    PeriodicMthdInLoop(void)
    {
        t_anchor = millis();
        t_current = millis();
        prev_flg = false;
        curr_flg = false;
    }
    PeriodicMthdInLoop(unsigned long period, unsigned long duration,
                       void (*f_fptr1)(void), void (*f_fptr2)(void))
        : t_period(period), t_duration(duration),
          fptr_dr(f_fptr1), fptr_dflt(f_fptr2)
    {
        t_anchor = millis();
        t_current = millis();
    }
    int checkCurrentStat(void);
    int checkCurrentStat(unsigned long, unsigned long, void (*)(void), void (*)(void));
};

#endif