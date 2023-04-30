/*************************************************************
 * MQTT import
 * Arduino Pro Mini
 * 4 channels control
   Blynk 서버가 없었던 자리에 MQTT 서버를 이용한다.
*************************************************************/

/*************************************************************
    import
        MQTT
        ESPSerial
        SoftwareSerial
*************************************************************/
#include <PubSubClient.h>   //https://pubsubclient.knolleary.net
#include <WiFiESP.h>        //https://github.com/bportaluri/WiFiEsp.git
#include <SoftwareSerial.h> //https://docs.arduino.cc/learn/built-in-libraries/software-serial
SoftwareSerial EspSerial(2, 3); // RX, TX

/*************************************************************
    define
      ESP8266_BAUD : ESP 모뎀 통신을 위한 소프트시리얼 보드레이트
      GH*_V ? : 온실과 연결된 핀 번호 정의
      MT_S : 모터 작동 릴레이. 오픈 컬렉터 회로 이용해야함 
*************************************************************/
#define ESP8266_BAUD 9600
#define GH1_V 8 //arduino pin 8
#define GH2_V 7 //arduino pin 7
#define GH3_V 6 //arduino pin 6
#define GH4_V 5 //arduino pin 5
#define MT_S 4  //arduino pin 4

/*************************************************************
    WiFiESP에서 붙여옴. 
    WiFi module 접속을 위한 처리
    WiFiESP코드를 보니 extern Espxxx WiFi로 정의되어 있음
      extern으로 어디가 먼저인지는 모르지만 두 헤더파일이 연결되어 있음
*************************************************************/
char ssid[] = "farmmain";            // your network SSID (name)
char pass[] = "wweerrtt";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

/*************************************************************
    자동/수동 관수를 위한 환경 변수
    Scope가 Global이 되기 위해서 == loop()안에서 쓰고 값을 유지하기 위해
*************************************************************/
int wateringTimeSec;    //1회 관수시간 sec
int periodMin;          //4동 전체 관주하며 돌아오는 관주 주기시간 min
bool GH[4] = {false}; //온실 각동의 밸브 ex: GH[0]은 1동의 valve 상태
bool MT = false;          //Motor 기동 스위치 on여부 수동일때만 고려
bool isAutoMode=false;  // isAutoMode에서는 MT상관 없이 자동모드 실행
bool prevAutoMode=false;     // prevAutoMode는 수동/자동 버튼의 이전 상태를 기억함.
unsigned long wateringStart;
unsigned long lastWateringTime;

void setup()
{
  // 디버깅을 위해 Serial을 초기화 한다.
  Serial.begin(115200);
/*************************************************************
   ESP modem을 통해 WPA형식의 암호 AP에 연결한다.
*************************************************************/
  EspSerial.begin(ESP8266_BAUD);
  // initialize ESP module
  WiFi.init(&EspSerial);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
}

void loop()
{

}