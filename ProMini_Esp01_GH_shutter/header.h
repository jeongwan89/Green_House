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

IPAddress server(192, 168, 0, 24);

SoftwareSerial EspSerial(10, 11); //Rx, Tx
WiFiClient espClient;   //wifiespAT에서 정의됨
PubSubClient client(espClient);
// 탱크 번호 정의
#define GH_SHUTTER_1

// MQTT 메세지 등 정의
#ifdef GH_SHUTTER_1
#define CLIENT_NAME     "iGH_SHUTTER_1"  //혹시나 싶어 쓴것. EspMQTTClient.h에 연동
#define MQTT_PUB_W1_CURR "Sensor/GH1/MTR_CURR/W1" //하우스1동 서편 1중
#define MQTT_PUB_W2_CURR "Sensor/GH1/MTR_CURR/W2" //하우스1동 서편 2중
#define MQTT_PUB_E1_CURR "Sensor/GH1/MTR_CURR/E1" //하우스1동 동편 1중
#define MQTT_PUB_E2_CURR "Sensor/GH1/MTR_CURR/E2" //하우스1동 동편 2중
#define MQTT_PUB_ERROR  "Error/GH1/MTR_CURR/Status" // 혼합통 A의 저수위 경고를 본다.
#define MQTTID          "id_GH_SHUTTER_1"
#define MQTTUSER        "farmmain"
#define MQTTPASS        "eerrtt"
#define WILLTOPIC_W1    "Lastwill/GH1/MTR_CURR/W1/Status"
#define WILLTOPIC_W2    "Lastwill/GH1/MTR_CURR/W2/Status"
#define WILLTOPIC_E1    "Lastwill/GH1/MTR_CURR/E1/Status"
#define WILLTOPIC_E2    "Lastwill/GH1/MTR_CURR/E2/Status"
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
        void Stop(void) 
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