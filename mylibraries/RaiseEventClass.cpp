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

// void RaiseTimeEventInLoop::operator()()
// {
//     static bool statusLED = 0;
//     statusLED = !statusLED;
//     digitalWrite(LED_BUILTIN, statusLED);   
// }

// void RaiseTimeEventInLoop::operator()(void (*fptr)(void))
// {
//     fptr();   
// }

void RaiseTimeEventInLoop::EachEveryTimeIn(unsigned long t_period, void (*fptr)(void))
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period 
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period) {
        //주기마다 실행해야할 함수를 작성한다.
        fptr();
        t_anchor = t_current;
    }
    t_current = millis();   
}

//=========================================================================================

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

int PeriodicMthdInLoop::CheckCurrentStat(void){
    // 조건식 t_current - t_anchor < t_period
    // yes : 지금은 주기 안에 있다는 이야기임
    // no : 주기 밖에 있으므로 얼른 초기화하고 돌아나가야 함
    if (t_current - t_anchor < t_period) {              // t_period안에 들어온 상황
        // 조건식 : t_current - t_anchor < t_duration
        // yes : 무엇인가 동작을 해야함
        // no : default 동작을 해야함
        if (t_current - t_anchor < t_duration) {            // t_duration 안에 들어오 상황
            curr_flg = true;
            // 무슨 동작을 해야하는데 루프마다 무슨 동작을 할 필요는 없다.
            // 단호하게 상태가 바뀌는 그때, 딱 한번만 해야한다.
            // 그러므로 아래와 같은 조건식이 필요하겠다.
            if ((!prev_flg)&&(curr_flg)){
                fptr_dr();
                prev_flg = curr_flg;
            }
            t_current = millis();
            return 1;
        } else {                                            // t_duration 밖에 상황
            curr_flg = false;
            // duration밖에 도달하는 default 동작도 한번만 하면된다.
            // 그래서 아래 식이 필요하겠다.
            if ((prev_flg)&&(!curr_flg)){
                fptr_dflt();
                prev_flg = curr_flg;
            }
            t_current = millis();
            return 0;
        }
    } else {                                            // t_period 밖의 상활
        // 다시 period(주기)가 시작되도록 인수들을 조정한다.
        t_anchor = millis();
        t_current = millis();
        return 0;
    }
}

// CheckCurrentStat의 인수용 버젼. 이 함수를 통하여 클래스 내에 있는 변수를 조정하고
// 계속 인수를 변경해서 쓸 수 있다. 이 말은 주기와 동작 시간과 동작 함수, 디폴트 함수를
// 루프 안에서 경우에 따라 다르게 쓸 수 있다는 이야기가 된다.
// 인수를 조정한 후 CheckCurrentStat(void) 함수를 그대로 이용하여 사용한다.
int PeriodicMthdInLoop::CheckCurrentStat(unsigned long period, unsigned long duration,
                                         void (*fptr1)(void), void (*fptr2)(void)) {
    t_period = period;
    t_duration = duration;
    fptr_dr = fptr1;
    fptr_dflt = fptr2;
    CheckCurrentStat();
}
