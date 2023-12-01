/*
    my time function을 테스트하기 위한 setup loop 함수
    호출되는 함수에서 static 키워드를 써서
    내가 원하는 시간에 함수를 호출하고 그 호출된 함수가 
    내가 동작을 원하는 함수를 호출한다.
    호출되는 조건과 상황은 조금씩 주석하여 양을 이어나간다.
*/


// 매 시간 마다 함수를 내가 원하는 함수를 호출한다.
// t_period : 이 함수가 해야할 일의 주기를 인수로 받는다.
void everyxxtime(unsigned long t_period, void (*fptr)(void))
{
    // t_anchor : static이고 초기는 이 함수가 실행되는 때의 시간.
    // 이후로는 period_t가 지난 후의 시스템 시간
    static unsigned long t_anchor = millis();
    // t_current : 루프 중에 시스템 시간을 계산한다.
    static unsigned long t_current = millis();

    // 조건식 : t_current-t_anchor > t_period 
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if ( t_current - t_anchor >= t_period) {
        //주기마다 실행해야할 함수를 작성한다.
        fptr();
        t_anchor = t_current;
    }
    t_current = millis();
}
// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    everyxxtime(1000, eventEveryxxTimeBlink);
}


// 주기마다 실행해야하는 함수
// 일종의 event 함수라고 생각하여 접두어에 event을 붙임
void eventEveryxxTime()
{
    static bool statusLED = 0;
    statusLED = !statusLED;
    digitalWrite(LED_BUILTIN, statusLED);
} 

void eventEveryxxTimeBlink(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
}