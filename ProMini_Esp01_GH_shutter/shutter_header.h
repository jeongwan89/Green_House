//================================================================================
// (드디어!!!) 하우스 창문 개폐기
// Hardware : ProMini 쓰고 Esp01 모뎀으로 MQTT 통신
// 왠만한 것은 OOP으로 프로그램한다.
// 모터의 상태를 확인하는 hardware가 반드시 있어야 된다.
//      쇼트인지, 리미트에 걸려서 작동을 하지 않는 것인지 판단한다.
//      INA219를 사용한다.
// 작동 : 
//      [v]MQTT에 subscribe! 명령받으면 작동/정지를 그대로 수행 (수동모드)
//          언제나 모터의 상태를 MQTT에 publish
//      []- 자동 모드
//          MQTT가 안정화되면, 온도에 subscribe
//          각각의 4 모터를 따로 작동하면서 온실의 온도를 일정하게 유지하도록 함
//          모터의 상태를 늘 확인하면서 MQTT와 완벽하게 연동되도록 설정할 것
//      []- 비상 모드
//          MQTT가 emergency 상태가 되면 모든 모터를 Up할 것
//          여전히 모터의 상태를 확인하고 MQTT에 보고할 것
// 모니터 : 전류를 늘 확인하면서 과전류이거나 전류가 흐르지 않는 것을 
//          MQTT에 publish한다.
//================================================================================

// INA219
#include <Wire.h>
#include <Adafruit_INA219.h>
        /*  Adafruit_INA219(uint8_t addr = INA219_ADDRESS);
            ~Adafruit_INA219();
            bool begin(TwoWire *theWire = &Wire);
            void setCalibration_32V_2A();
            void setCalibration_32V_1A();
            void setCalibration_16V_400mA();
            float getBusVoltage_V();
            float getShuntVoltage_mV();
            float getCurrent_mA();
            float getPower_mW();
            void powerSave(bool on);
            bool success(); */

// ESP01 모뎀 설정과 MQTT lib
#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#include <stdlib.h>
#include <stdio.h>
#define ESP8266_BAUD 57600

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

// wifi 연결 선언과 mqtt 연결 객체 선언

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(10, 11); //Rx, Tx
WiFiClient espClient;   //wifiespAT에서 정의됨
PubSubClient client(espClient);

// 탱크 번호 정의, 어떤 액체 탱크이지요?
#define GH_SHUTTER_1

// MQTT 메세지 등 정의
#ifdef GH_SHUTTER_1
#define CLIENT_NAME     "iGH_SHUTTER_1"  //혹시나 싶어 쓴것. EspMQTTClient.h에 연동
#define MQTTID          "id_MTR_CTRL_GH1_MQTT"
#define MQTT_PUB_E1_CURR "Sensor/GH1/MTR_CURR/E1" //하우스1동 동편 1중
#define MQTT_PUB_E2_CURR "Sensor/GH1/MTR_CURR/E2" //하우스1동 동편 2중
#define MQTT_PUB_W1_CURR "Sensor/GH1/MTR_CURR/W1" //하우스1동 서편 1중
#define MQTT_PUB_W2_CURR "Sensor/GH1/MTR_CURR/W2" //하우스1동 서편 2중
#define MQTT_SUB_E1_CMD "Command/GH1/MTR/W1" // 하우스1동 동편 1중 동작 명령
#define MQTT_SUB_E2_CMD "Command/GH1/MTR/W2" // 하우스1동 동편 2중 동작 명령
#define MQTT_SUB_W1_CMD "Command/GH1/MTR/E1" // 하우스1동 서편 1중 동작 명령
#define MQTT_SUB_W2_CMD "Command/GH1/MTR/E2" // 하우스1동 서편 2중 동작 명령
#define MQTT_E1_MTR_CURR_ERROR  "Error/GH1/MTR_CURR/E1" // 모터 over current이냐? 리미트에 걸렸나?
#define MQTT_E2_MTR_CURR_ERROR  "Error/GH1/MTR_CURR/E2"
#define MQTT_W1_MTR_CURR_ERROR  "Error/GH1/MTR_CURR/W1"
#define MQTT_W2_MTR_CURR_ERROR  "Error/GH1/MTR_CURR/W2"
#define MQTTUSER        "farmmain"
#define MQTTPASS        "eerrtt"
#define WILLTOPIC       "Lastwill/GH1/MTR_CURR/Status"
#define WILLMSG         "off line"
#endif

// Shutter 클래스
// 모터의 작동을 결정한다.
// 모터의 방향을 결정한다.
// ProMini와 연결된 핀을 인수로 받는다.
//      Relay가 두 개가 있으므로, pin이 2개 필요하다.
//      결선을 반대로 할 수 있으니까 작동을 반전시킬 flag가 필요하다. -> invDir
class Shutter : public Adafruit_INA219
{
    public:
        // 아래 두 bool값은 모터의 상태를 알기위한 flag이다. 
        // 객체가 되었을 때, 작동하는지 방향 설정을 어떻게 되었는지에 대한 정보를 제공한다.
        bool motor = false; // 모터의 작동 (방향은 모른다. 일단 작동만 한다. true : 작동, false : 정지)
        bool upDir = false; // 모터의 방향 (작동은 모른다. 일당 방향만 정한다. true : up, false : down)
        // 사실 필요없는 일인지도 모르겠다.
        // 나중을 위해 살려놓는데, 용량이 작으면 그냥 주석처리

        uint8_t motorPin;
        uint8_t upDirPin;
        bool invDir = false;    // 방향 flag 늘 배선을 반대로 할 수 있으니까 방향을 MQTT로 받아서 작동을 반전시킨다.
    public:
        // Adafruit_INA219(Addr) 생성자의 Addr 정하는 납땜
        // A1       A0          inr219 address
        // open     open        0x40
        // open     bridged     0x41
        // bridged  open        0x44
        // bridged  bridged     0x45
        // constructor. Only one
        Shutter(uint8_t mtrPin, uint8_t upDrPin, uint8_t currAddr=0x40) : Adafruit_INA219(currAddr)
        {

            motorPin = mtrPin;
            upDirPin = upDrPin;
            pinMode(motorPin, OUTPUT);
            pinMode(upDirPin, OUTPUT);
            begin();    // Adafruit에서 상속받은 함수이다. this->를 쓸까?
        }
        
        // 모터를 스톱한다. 방향 flag는 손대지 않는다.
        void stop(void) 
        {
            digitalWrite(motorPin, 0);
            motor = false;
        }

        // 방향 flag를 논리연산으로 구한다.
        //  pin             flag                result
        //  0(내려라)       0(반전아니다)       0(내린다)
        //  1(올려라)       0(반전아니다)       1(올린다)
        //  0(내려라)       1(반전이다)         1(올린다)
        //  1(올려라)       1(반전이다)         0(내린다)
        //  연산의 결과는 xor이므로 비트연산자 ^ (exclusive OR)을 사용한다.

        // window를 올린다.
        void Up(void) 
        {
            digitalWrite(upDirPin, HIGH ^ invDir);
            digitalWrite(motorPin, HIGH);
            upDir = true;
            motor = true;
        }

        // window를 내린다.
        void Down(void) 
        {
            digitalWrite(upDirPin, LOW ^ invDir);
            digitalWrite(motorPin, HIGH);
            upDir = false;
            motor = true;
        }
};

// RaiseTimeEventInLoop 클래스
// 목적: loop()의 실행에 상관없이 주기로 정해진 시간에
//      인자로 받은 함수를 한번 실행한다. 
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

//============================================================================
// reset 처리 구간
//============================================================================

// resetFunction
void (*resetFunc)(void) = 0; //리셋함수는 어드레스가 0부터 시작하는 함수이다.

//=============================================================================
// ESP01 함수 정의, 
//      wifi연결 : wifiConnect
//      mqtt연결 : 
//=============================================================================

// Wifi 연결 함수
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

/*
    MQTT subscribe 함수이다.
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

/**
* @brief  MQTT에 연결한다.
* @param  void 인자값은 없다.
*/
void reconnect(void) {
    int errCnt = 0;
    while(!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if(client.connect(MQTTID,MQTTUSER,MQTTPASS,WILLTOPIC, 0, 1, WILLMSG)) {
            Serial.print("connected");
            client.publish(WILLTOPIC, "on line", 1);
            // client.subscribe("...");
            client.subscribe(MQTT_SUB_E1_CMD);
            client.subscribe(MQTT_SUB_E2_CMD);
            client.subscribe(MQTT_SUB_W1_CMD);
            client.subscribe(MQTT_SUB_W2_CMD);
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
