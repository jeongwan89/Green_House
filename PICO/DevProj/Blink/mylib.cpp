#include "Arduino.h"
#include "mylib.h"

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period, void (*fptr)(void))
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period)
    {
        // 주기마다 실행해야할 함수를 작성한다.
        fptr();
        t_anchor = t_current;
    }
    t_current = millis();
}

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period,
                                           void (*fptr)(const char *, const char *), const char *arg_a, const char *arg_b)
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period)
    {
        // 주기마다 실행해야할 함수를 작성한다.
        fptr(arg_a, arg_b);
        t_anchor = t_current;
    }
    t_current = millis();
}

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period,
                                           void (*fptr)(const char *, int), const char *arg_a, int arg_b)
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period)
    {
        // 주기마다 실행해야할 함수를 작성한다.
        fptr(arg_a, arg_b);
        t_anchor = t_current;
    }
    t_current = millis();
}


// checkCurrentStat의 인수(arguement)전용 버젼. 이 함수를 통하여 클래스 내에 있는 변수를 조정하고
// 계속 인수를 변경해서 쓸 수 있다. 이 말은 주기와 동작 시간과 동작 함수, 디폴트 함수를
// 루프 안에서 경우에 따라 다르게 쓸 수 있다는 이야기가 된다.
// 인수를 조정한 후 checkCurrentStat(void) 함수를 그대로 이용하여 사용한다.
int PeriodicMthdInLoop::checkCurrentStat(unsigned long period, unsigned long duration,
                                         void (*fptr1)(void), void (*fptr2)(void))
{
    // period: 동작 주기; duration: 동작 주기 동안 행해지는 실행 시간
    t_period = period;
    t_duration = duration;
    fptr_dr = fptr1;
    fptr_dflt = fptr2;
    return checkCurrentStat();
}

/**
 * @brief 이 함수를 알기위해서는 주기와 동작시간의 상관관계를 알아야하겠다.
 * 주기period 속 동작시간duration 안에 있으면 연결된 함수를 실행한다.
 * 
 * @return int 1: 주기period 속 동작시간duration에 있고 동작 함수를 실행하였다. 
 * int 0: 주기period 속이지만 동작시간이 끝나고 빈 공백 함수를 실행하였다.
 */
int PeriodicMthdInLoop::checkCurrentStat(void)
{
    // 조건식 t_current - t_anchor < t_period
    // yes : 지금은 주기 안에 있다는 이야기임
    // no : 주기 밖에 있으므로 얼른 초기화하고 돌아나가야 함
    if (t_current - t_anchor < t_period)
    { // t_period안에 들어온 상황
        // 조건식 : t_current - t_anchor < t_duration
        // yes : 무엇인가 동작을 해야함
        // no : default 동작을 해야함
        if (t_current - t_anchor < t_duration)
        { // t_duration 안에 들어오 상황
            curr_flg = true;
            // 무슨 동작을 해야하는데 루프마다 무슨 동작을 할 필요는 없다.
            // 단호하게 상태가 바뀌는 그때, 딱 한번만 해야한다.
            // 그러므로 아래와 같은 조건식이 필요하겠다.
            if ((!prev_flg) && (curr_flg))
            {
                fptr_dr();
                prev_flg = curr_flg;
            }
            t_current = millis();
            return 1;
        }
        else
        { // t_duration 밖에 상황
            curr_flg = false;
            // duration밖에 도달하는 default 동작도 한번만 하면된다.
            // 그래서 아래 식이 필요하겠다.
            if ((prev_flg) && (!curr_flg))
            {
                fptr_dflt();
                prev_flg = curr_flg;
            }
            t_current = millis();
            return 0;
        }
    }
    else
    { // t_period 밖의 상활
        // 다시 period(주기)가 시작되도록 인수들을 조정한다.
        t_anchor = millis();
        t_current = millis();
        return 0;
    }
}
