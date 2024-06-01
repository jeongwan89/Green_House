//============================================================================================
//      function.cpp
//============================================================================================
#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>
#include <stdio.h>
#define ESP8266_BAUD 57600

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(8, 7); //Rx, Tx
WiFiClient espClient;   //WiFiEspAT에서 정의됨
PubSubClient client(espClient);

// MQTT 메세지 등 정의
#define MQTTID      "levelMonitorAtank"
#define MQTTUSER    "farmmain"
#define MQTTPASS    "eerrtt"
#define WILLTOPIC   "Lastwill/FM/FerTankA/Status"
#define WILLMSG     "off line"

// pin 번호 정의
#define TRIG 2
#define ECHO 3
#define LEVEL_LIMIT 5
#define ERR_RELAY 13

// 탱크 번호 정의
#define LEVEL_TANK_A

#ifdef LEVEL_TANK_A
#define CLIENT_NAME     "LevelMonitor_TankA"  //혹시나 싶어 쓴것. EspMQTTClient.h에 연동
#define MQTT_PUB_LEVEL  "Sensor/FM/FerTankA/Level" //FM 작업동, fertankA 혼합통A
#define MQTT_PUB_ERROR  "Error/FerTankA/Status" // 혼합통 A의 저수위 경고를 본다.
#endif

// resetFunction
void (*resetFunc)(void) = 0; //리셋함수는 어드레스가 0부터 시작하는 함수이다.

/*
    이 콜백에서는 따로 payload뿐만 아니라 str[256]에서 카피를 받아쓴다.
    payload마지막에 NULL이 있는지 없는지는 모르겠지만,
    str에는 마지막에 NULL이 있어서 string으로 쓸수 있다.
*/
void callback(char* topic, byte* payload, unsigned int length) {
    char str[256];
    int conv;
    Serial.print("Message arrived []");
    Serial.print(topic);
    Serial.print("]");
    for(int i = 0; i < length; i++){
        Serial.print((char) payload[i]);
        str[i] = (char) payload[i];
        str[i+1] = NULL;
    }
    // 이하 특별한 topic message에 대한 처리 루틴이 있어야한다.
}

void reconnect(void) {
    int errCnt = 0;
    while(!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if(client.connect(MQTTID,MQTTUSER,MQTTPASS,WILLTOPIC, 0, 1, WILLMSG)) {
            Serial.print("connected");
            client.publish(WILLTOPIC, "on line", 1);
            // client.subscribe("...");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print("\t try again in 5 seconds\n");
            delay(5000);
            if(errCnt > 10) resetFunc();
            else errCnt++;
        }
    }
}

void wifiConnect(void) 
{
/* setup() 안에서 작동한다. 이전에 반드시 정의 되어야할 것이 있는데, 
    ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
    WiFi 쉴드 변수와 ESP01모뎀의 객체의 레퍼런스를 연결해 주어야 한다.
*/
    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        delay(100);
        resetFunc();
    }

    Serial.print("Attempting to connect to WPA SSID:");
    Serial.println(ssid);
    
    WiFi.begin(ssid, pass);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Your're connected to the networtk");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

int sumDist = 0;
int avrDistance(unsigned int distance)
{
    static unsigned int arrDistance[128] = {0,};
    static int index = 0;

    if(index == 128) index=0; //다음 주기 첫번째 0
    sumDist = sumDist - arrDistance[index] + distance;
    arrDistance[index] = distance;
    index ++;
    //Serial.print("index :"); Serial.println(index);
    //Serial.print("arrDistance["); Serial.print(index); Serial.print("] = "); Serial.println(arrDistance[(index == 0 ? 127 : index-1)]);
    //Serial.println(sumDist);
    return (int) (sumDist/127);
}

// 사각 탱크의 남은 용적을 구한다. 일단 100세티로 계산하고 거리를 받아서 %로 환산해서 리턴한다.
int volumeRemain(int dist)
{
    return (85 - dist); //112.5cm는 물탱크의 깊이
}

// 초음파 센서로 거리를 측정한다.
// mesureDistance()
// arguments:
//      echo : 초음파 센서 출력부
//      trig : 초음파 센서 수신부
//      위의 두 핀은 setup()에서 pinMode가 미리 정의되어 있어야 한다.
int ultraSonicMeasureDistance(int trig, int echo)
{
    unsigned long duration;
    unsigned int distance;
    unsigned int averageDistance;

    //초음파 스피커에 펄스를 준다. 0.01초 (3.4005m 정도 갔다)
    digitalWrite(trig, HIGH);
    delay(10);
    digitalWrite(trig, LOW);

    // 핀에서 펄스(HIGH 또는 LOW)를 읽습니다. 
    // 예를 들어, value 가 HIGH 이면, pulseIn() 은 핀이 HIGH` 가 될 때까지 기다리고, 
    // 타이머를 시작하고, 핀이 LOW 가 될 때까지 기다리고 타이머를 멈춥니다. 
    // 펄스의 길이를 마이크로초 단위로 반환합니다. 
    // 정해진 timeout 안에 펄스가 시작되지 않으면 0을 반환합니다. 
    // 이 함수의 타이머는 경험적으로 결정되고, 긴 펄스에 대해 오류를 낼 수도 있습니다. 
    // 10 마이크로초부터 3분까지의 길이의 펄스에 대해 동작합니다.
    duration = pulseIn(echo, HIGH);
    distance = (unsigned long) ((34045 * duration) / 1000000) / 2;

    averageDistance = avrDistance(distance);
    
    // debug
    Serial.print(averageDistance);
    Serial.println("cm");

    return distance;
}

// MQTT에 쓰기 위해서 루프마다 쓸 필요는 없다. 
// 그 상태가 바뀌었을 때, 루프를 쓰는 것이 좋겠다.
// 그러므로 이하는 상태를 인수로 받아 바뀐 것을 판단하고 어떤 행동을 하려는 함수이다.
bool isChanged(bool prev, bool current) {
    if(prev != current) return true;
    else return false;
}

// 0: 아무 변화도 없었다.
// 1: 변화가 있었다. 물이 없다는 변화
// 2: 물이 있다는 변화
int isChangedDoSomething(bool * prev, bool * current)
{
    // 기계적으로 이번 루프에서 변화값을 쉬프트한다.
    * prev = * current;
    * current = digitalRead(LEVEL_LIMIT);

    // 상태가 같은가?
    // yes : 같다. 아무것도 안한다.
    // no : 다르다. 다르면 current 상태에 따라 MQTT 메세지를 다르게 송부한다.
    if(* prev == * current) return 0;
    else{
        // true : 물이 없다.
        // false : 물이 감지 된다.
        if(* current) {
            client.publish(MQTT_PUB_ERROR, "No fertile A", 1);
            digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(ERR_RELAY, HIGH);
            return 1;
        } else {
            client.publish(MQTT_PUB_ERROR, "None", 1);
            digitalWrite(LED_BUILTIN, LOW);
            digitalWrite(ERR_RELAY, LOW);
            return 2;
        }
    }
}
// ProMini_Esp01_levelMonitor_display.ino에만 쓰이는 특별함수
// class RaiseTimeEventInLoop의 instance "mqttPub"에 사용된다.
void doThis(const char* tpc, const char* msg)
{
    client.publish(tpc, msg, 1);
}

void doThis(const char* tpc, int msg)
{
    char buffer[128];
    itoa(msg, buffer, 10);
    client.publish(tpc, buffer, 1);
}

//============================================================================================
//     TM1637.h
//============================================================================================
#include <TM1637Display.h>

#define TM1637CLK   10
#define TM1637DIO   11

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display(TM1637CLK, TM1637DIO);

//=============================================================================================
//          myClassDev.h
//=============================================================================================

/*
    Arduino에서 함수를 인자로 호출하는 Class작성
    이 class는 loop()에 들어가기는 하지만, 정해진 주기에 한번 이 함수를 실행한다.
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
        void (*pfunct) (void); //생성자에서 실행할 함수를 기억할 용도로 쓸 예정이었는데, 그 효용성이 떨어졌다. 하위 버전 호환성을 위해 그냥 둔다.
    
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
        void eachEveryTimeIn(unsigned long t_period) { eachEveryTimeIn((t_period), pfunct); }
        void eachEveryTimeIn(unsigned long t_period, void (*fptr)(void));
        void eachEveryTimeIn(unsigned long t_period, void (*fptr)(const char *, const char *), const char *, const char *);
        void eachEveryTimeIn(unsigned long, void(*fptr)(const char*, int), const char*, int);
        void eachEveryTimeIn(int t_period, void(*fptr)(void)) { eachEveryTimeIn((unsigned long) t_period, fptr);}
        void eachEveryTimeIn(int t_period, void(*fptr)(const char*, const char*), const char* a, const char* b) {
             eachEveryTimeIn((unsigned long) t_period, fptr, a, b);}
        void eachEveryTimeIn(int t_period, void(*fptr)(const char*, int), const char* arg_a, int arg_b) {
             eachEveryTimeIn((unsigned long) t_period, fptr, arg_a, arg_b);}

        
        // void operator()();
        // void operator()(void (*fptr)(void));
};

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period, void (*fptr)(void))
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

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period, 
                                            void (*fptr)(const char*, const char*), const char* arg_a, const char* arg_b)
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period 
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period) {
        //주기마다 실행해야할 함수를 작성한다.
        fptr(arg_a, arg_b);
        t_anchor = t_current;
    }
    t_current = millis();   
}

void RaiseTimeEventInLoop::eachEveryTimeIn(unsigned long t_period, 
                                            void (*fptr)(const char*, int), const char* arg_a, int arg_b)
{
    // 조건식 : refClass.t_current-refClass.t_anchor > t_period 
    // yes : t_period보다 더 많은 시간이 흘렀다는 이야기
    // no : t_period보다 더 적으니 시간이 찰 때까지 기다려야한다는 이야기
    // 이 조건식의 두 조건 모두 t_current는 millis()의 반환값을 동시에 가져야 다음 호출에서 의미를 가짐
    if (t_current - t_anchor >= t_period) {
        //주기마다 실행해야할 함수를 작성한다.
        fptr(arg_a, arg_b);
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
        int checkCurrentStat(void);
        int checkCurrentStat(unsigned long, unsigned long, void (*)(void), void(*)(void));
};

int PeriodicMthdInLoop::checkCurrentStat(void){
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

// checkCurrentStat의 인수용 버젼. 이 함수를 통하여 클래스 내에 있는 변수를 조정하고
// 계속 인수를 변경해서 쓸 수 있다. 이 말은 주기와 동작 시간과 동작 함수, 디폴트 함수를
// 루프 안에서 경우에 따라 다르게 쓸 수 있다는 이야기가 된다.
// 인수를 조정한 후 checkCurrentStat(void) 함수를 그대로 이용하여 사용한다.
int PeriodicMthdInLoop::checkCurrentStat(unsigned long period, unsigned long duration,
                                         void (*fptr1)(void), void (*fptr2)(void)) {
    t_period = period;
    t_duration = duration;
    fptr_dr = fptr1;
    fptr_dflt = fptr2;
    checkCurrentStat();
}

//=========================================================================================

/*
void blink(void)
{
    static bool statusLED = 0;
    statusLED = !statusLED;
    digitalWrite(LED_BUILTIN, statusLED);   
}


RaiseTimeEventInLoop check; //전역으로 선언해야 loop()에서 보인다.
PeriodicMthdInLoop blinkLED(500, 100, blink, blink);

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    // check.eachEveryTimeIn(50, blink);
    blinkLED.checkCurrentStat(1000,100, blink, blink);
}
*/

