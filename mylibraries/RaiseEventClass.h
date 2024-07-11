#ifndef _RAISEEVENTCLASS_H_
#define _RAISEEVENTCLASS_H_

/*
    Arduino에서 함수를 인자로 호출하는 Class작성
*/
//==========================================================================================

// 루프 안에서 일정 주기가 돌아오면 그 주기 시각에 어떤 일을 하는 객체
// ex. 주기 5000 millis 마다 LED를 토글한다.
// ex. 주기 10,000 millis 마다 온도 센서를 읽는다.
class RaiseTimeEventInLoop {
    public: 
        // t_anchor : static이고 초기는 이 함수가 실행되는 때의 시간.
        // 이후로는 period_t가 지난 후의 시스템 시간
        unsigned long t_anchor;
        // t_current : 루프 중에 시스템 시간을 계산한다.
        unsigned long t_current;
        void (*pfunct) (void);
    public:
        RaiseTimeEventInLoop(){
            t_anchor = millis();
            t_current = millis();
        }
        RaiseTimeEventInLoop(void (*fptr)(void)){
            t_anchor = millis();
            t_current = millis();
            pfunct = fptr;
        }
        void EachEveryTimeIn(unsigned long t_period) { EachEveryTimeIn((t_period), pfunct); }
        void EachEveryTimeIn(unsigned long t_period, void (*fptr)(void));
        void EachEveryTimeIn(int t_period, void(*fptr)(void)) { EachEveryTimeIn((unsigned long) t_period, fptr);}

        
        // void operator()();
        // void operator()(void (*fptr)(void));
};

// 루프에서 어떤 시간동안 어떤 동작을 하는 클래스.
// ex. 60초 주기 5초동안에 분사하는 동작
// 필요요소: 주기, 동작시간, 동작정의함수
class PeriodicMthdInLoop {
    // member variable
    public:
        unsigned long t_period, t_duration, t_anchor, t_current;
        void (*fptr_dr) (void);
        void (*fptr_dflt) (void);
    private:
        bool prev_flg, curr_flg;  // 매개 함수 실행을 딱 한번 하기 위한 상태값
    // member function
    public:
        // constructor
        PeriodicMthdInLoop(void) {
            t_anchor = millis();
            t_current = millis();
            prev_flg = false;
            curr_flg = false;
        }
        PeriodicMthdInLoop(unsigned long period, unsigned long duration, 
                           void (*f_fptr1) (void), void (*f_fptr2) (void)) 
                          : t_period(period), t_duration(duration), 
                            fptr_dr(f_fptr1), fptr_dflt(f_fptr2) {
            t_anchor = millis();
            t_current = millis();
        }
        int CheckCurrentStat(void);
        int CheckCurrentStat(unsigned long, unsigned long, void (*)(void), void(*)(void));
};

#include "/home/kjw/Git/Green_House/mylibraries/RaiseEventClass.cpp"
#endif