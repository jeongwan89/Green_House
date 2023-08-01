/*************************************************************
    IMPORT :
        WiFiESP.h
        Pubsubclient.h
        SoftwareSerial.h

        stdlib.h
**************************************************************/
#include <WiFiEspAT.h>          //https://github.com/JAndrassy/WiFiEspAT
#include <PubSubClient.h>       //https://github.com/knolleary/pubsubclient
#include <SoftwareSerial.h>

#include <stdlib.h>
/*************************************************************
    My WiFi credentials (WPA) 이것은 WiFiEsp.h에 extern으로 정의되어 있음
**************************************************************/
#define ESP8266_BAUD 57600
SoftwareSerial EspSerial(2,3);  //RX, TX
char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress server(192, 168, 0, 24);  //mqtt에서 필요한 서버 IP
WiFiClient espClient;               // ─┬─ 이것과 아래 2개는 쌍으로 작동한다.
PubSubClient client(espClient);     //  └─ PubSubClient.h에서 필요한 것임

// MQTT authentification
#define MQTTID "mistcontrol"
#define MQTTUSER "farmmain"
#define MQTTPASS "eerrtt"

#define GH1_V 8 //arduino pin 8
#define GH2_V 7 //arduino pin 7
#define GH3_V 6 //arduino pin 6
#define GH4_V 5 //arduino pin 5
#define MT_S 4  //arduino pin 4

/*************************************************************
    MQTT argument. Blynk를 썼을 때 virtual button으로 사용하였던 것을
        MQTT 서버로 바꾸고, 거기서 -m message로 받을 인수로 쓴다.
        MQTT client 프로그램을 여기에 대응하도록 짜야 한다. 
*************************************************************/
int wateringTimeSec;    //1회 관수시간 sec
int periodMin;          //4동 전체 관주하며 돌아오는 관주 주기시간 min
bool GH[5] = {false}; //온실 각동의 밸브 ex GH[1]은 1동의 valve 상태
bool MT = false;          //Motor 기동 스위치 on여부 수동일때만 고려
bool isAutoMode=false;  // isAutoMode에서는 MT상관 없이 자동모드 실행
bool prevAutoMode=false;     // prevAutoMode는 수동/자동 버튼의 이전 상태를 기억함.
unsigned long wateringStart;
unsigned long lastWateringTime;

/*************************************************************
    필요한 함수를 정의하는 곳
**************************************************************/
// Automode가 선택되었을 때, callback에서 사용할 함수
// 몇가지 전역 변수가 필요해서 함수 위에다 정의한다.
int whatGH[5] = {0}; //어떤 온실을 관수하는가?
int NoGH; //관수해야할 온실의 동수는 몇동인가?
void automode(){  //수동 또는 누르면 자동
    NoGH = 0;
    //debug
    //Serial.println("Now entered automode()");
    //debug
    if(isAutoMode) {
        //debug
        //Serial.print("isAutomode is :"); Serial.println(isAutoMode);
        digitalWrite(LED_BUILTIN, HIGH);
        //debug
        wateringStart = millis(); //자동 버튼이 눌러지면 wateringStart를 초기화
        digitalWrite(GH1_V, LOW);
        digitalWrite(GH2_V, LOW);
        digitalWrite(GH3_V, LOW);
        digitalWrite(GH4_V, LOW);
        digitalWrite(MT_S, LOW);
        int j = 1;  //whatGH[]의 인수로 쓰임. 
        for(int i = 1; i <= 4; i++){
          // whatGH[i]는 순차적으로 해야하는 관수동을 인수로 집어넣은 배열이다.
          // whatGH[i]의 인수가 0이면 i-1에서 관수가 끝이나는 의미이다.
          // whatGH[i]의 인수가 0이 아니라면, 그 인수가 지금 관수하는 동의 번호이다.
          // whatGH[i]의 인수와 i의 수자가 인수를 확인하며 관수해야 한다.
          if(GH[i] == true) { 
            whatGH[j] = i;
            j++;
            whatGH[j] = 0;
            NoGH++;
          } //조건부를 거치고 나면 whatGH[]의 인수는 관수해야할 동수를 나타낸다.
        }
    } 
    else { //수동모드일때 LED 끄고 다음 callback을 기다린다.
      //debug
      //Serial.print("isAutomode is :"); Serial.println(isAutoMode);
      digitalWrite(LED_BUILTIN, LOW);
      //debug
    }
    //debug
    //Serial.println("Now exit automode()");
    //debug
    prevAutoMode = isAutoMode;
}
//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
    char str[256];
    int conv;
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
        Serial.print((char)payload[i]);
        str[i] = (char) payload[i];
        str[i+1] = NULL;
    }
    Serial.println();
    if (strcmp(topic, "Argument/WRS/mistcontrol/auto") == 0) {
        conv = atoi(str);
        isAutoMode = (bool) conv;
        //isAutoMode = (bool) payload;
        //debug payload값을 알아야 하겠다.
        //Serial.print("The value of conv is :"); Serial.println(conv);
        //Serial.print("The length of payload is :"); Serial.println(length);
        //debug
        //자동이 눌러졌을 때 필요한 process가 있으므로 함수로 넘겨 사용
        automode();
        }
    else if(strcmp(topic,"Argument/WRS/mistcontrol/freq") == 0)
        periodMin = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/dura") == 0)
        wateringTimeSec = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/motor") == 0)
        MT = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/GH1") == 0)
        GH[1] = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/GH2") == 0)
        GH[2] = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/GH3") == 0)
        GH[3] = atoi(str);
    else if(strcmp(topic, "Argument/WRS/mistcontrol/GH4") == 0)
        GH[4] = atoi(str);
}
void MotorOn(){
    digitalWrite(MT_S, HIGH); // MT_S는 모터에 연결된 핀 번호
}
void MotorOff(){
    digitalWrite(MT_S, LOW);
}
//각 동마다 관수. 
void wateringThisGH(int thisGH) 
{
    switch (thisGH)
    {
    case 1 : /* constant-expression */
        /* code */
        digitalWrite(GH1_V, HIGH);
        digitalWrite(GH2_V, LOW);
        digitalWrite(GH3_V, LOW);
        digitalWrite(GH4_V, LOW);
        break;
    case 2 :
        digitalWrite(GH1_V, LOW);
        digitalWrite(GH2_V, HIGH);
        digitalWrite(GH3_V, LOW);
        digitalWrite(GH4_V, LOW);
        break;
    case 3 :
        digitalWrite(GH1_V, LOW);
        digitalWrite(GH2_V, LOW);
        digitalWrite(GH3_V, HIGH);
        digitalWrite(GH4_V, LOW);
        break;
    case 4 :
        digitalWrite(GH1_V, LOW);
        digitalWrite(GH2_V, LOW);
        digitalWrite(GH3_V, LOW);
        digitalWrite(GH4_V, HIGH);
        break;
    default: //thisGH가 1~4범위를 벗어나면 모든 동의 관수 밸브를 닫음
        digitalWrite(GH1_V, LOW);
        digitalWrite(GH2_V, LOW);
        digitalWrite(GH3_V, LOW);
        digitalWrite(GH4_V, LOW);
        break;
    }
}

void setup()
{
    // initialize serial for debugging
    Serial.begin(115200);
    // initialize serial for ESP module
    EspSerial.begin(ESP8266_BAUD);
    // initialize ESP module
    WiFi.init(&EspSerial);

    // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // don't continue
        // (∨)이 상황에서 WiFi 쉴드가 없다는 사실을 mqtt에 알려야 하지 않을까?
        //  └─ 쉴드가 없는데 어떻게 mqtt에 알릴 수 있는가. 없다!
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
    // (∨)연결되었으니 mqtt에 연결되었음을 신고하고, 유언 메세지도 등록한다.
    //  └─ MQTT에 연결이 안되었으니 이런 시도는 무산될 것임. 작동한할 것임으로 시도하지 않음
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    //connect to MQTT server
    client.setServer(server, 1883);
    client.setCallback(callback);

/* reconnect()가 loop()에서 의도치 않게 연결되지 않을 때, 미리 setup()에서 구독함수를
    정의하고 시작하는 프로시져를 생각했는데, 이것은 abondant 코드임 나중에 삭제해도 되나
    들였던 공을 생각해서 남겨둔다.
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
 
        if (client.connect(MQTTID, MQTTUSER, MQTTPASS)) {
            Serial.println("connected " + String(client.state()));
        } 
        else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    client.subscribe("Argument/WRS/mistcontrol/auto");
    client.subscribe("Argument/WRS/mistcontrol/freq");
    client.subscribe("Argument/WRS/mistcontrol/dura");
    client.subscribe("Argument/WRS/mistcontrol/motor");
    client.subscribe("Argument/WRS/mistcontrol/GH1");
    client.subscribe("Argument/WRS/mistcontrol/GH2");
    client.subscribe("Argument/WRS/mistcontrol/GH3");
    client.subscribe("Argument/WRS/mistcontrol/GH4");
*/
/*************************************************************
    pin모드 초기화(setup() 속에 있다.)
**************************************************************/    
    pinMode(GH1_V, OUTPUT);
    pinMode(GH2_V, OUTPUT);
    pinMode(GH3_V, OUTPUT);
    pinMode(GH4_V, OUTPUT);
    pinMode(MT_S, OUTPUT);
    //debug
    pinMode(LED_BUILTIN, OUTPUT);
}

/*************************************************************
    loop()에서 필요한 함수를 정의하는 곳
**************************************************************/
void reconnect() 
/* 
    일종의 재귀적 용법의 함수 같아 보인다.
    client.connect()가 한번도 실행되지 않았는데 loop()안에서 reconnect()가 실행되고
    나서야 그 안의 client.connect()가 실행된다. 
    장접 : 연결이 끊기면 언제나 이 함수에 들어와서 connect() 실행한다.
    중요점: publish(const char * topic, const char * payload)
            subscribe(const char * topic)
*/
{
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect, just a name to identify the client
        if (client.connect(MQTTID, MQTTUSER, MQTTPASS)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            //client.publish("command","hello world");
            // ... and resubscribe
            //client.subscribe("presence");
            client.subscribe("Argument/WRS/mistcontrol/freq");
            client.subscribe("Argument/WRS/mistcontrol/dura");
            client.subscribe("Argument/WRS/mistcontrol/motor");
            client.subscribe("Argument/WRS/mistcontrol/GH1");
            client.subscribe("Argument/WRS/mistcontrol/GH2");
            client.subscribe("Argument/WRS/mistcontrol/GH3");
            client.subscribe("Argument/WRS/mistcontrol/GH4");
            client.subscribe("Argument/WRS/mistcontrol/auto");
            // debug
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/auto");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/freq");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/auto");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/motor");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/GH1");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/GH2");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/GH3");
                Serial.print("subscribe"); Serial.print(" : "); Serial.println("Argument/WRS/mistcontrol/GH4");
            // debug
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
    }
}

int wateringCount_th = 1;   // loop() 안에서 쓸 전역변수
                            // 관수 주기에서 몇번째로 관수하고 있는지 확인
void loop()
/*
    두 파트으로 나눈다.
    파트1 : loop()중에 연결이 확실한지 처리한다. 
            reconnect()에 MQTT topic을 늘 재정의한다.
            mistcontrol 특성상 언제나 mqtt 서버에 연결될 수는 없다. 
            서버에서 떨어지면 즉시 연결하는 정책을 감수해야하겠다.
                서버에서 떨어졌을 때, 명령이 오는지 안오는지 알 방법이 없으므로
                늘 서버에 붙어 있는 것이 낫다.
    파트2 : pocket이 있다. 이 포켓은 programmed watering을 실행하는 곳이다.
            이 포켓에 들어가기 위한 전역변수들을 정의해야 한다.
*/
{
    //파트1
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
        while ( status != WL_CONNECTED) {
            Serial.print("Attempting to connect to WPA SSID: ");
            Serial.println(ssid);
            // Connect to WPA/WPA2 network
            status = WiFi.begin(ssid, pass);
            delay(500);
        }
        Serial.println("Connected to AP");
        IPAddress ip = WiFi.localIP();
  
        Serial.print("IP Address: ");
        Serial.println(ip);
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    //delay(100); //MQTT 요청을 안정화하기 위해 쓴 delay. subscribe가 되지 않았을 때 속도를 늦추어서 처리되는 양을 천천히 하고자 했다.
        
    //파트2
    if(!isAutoMode){  // 수동 모드
        digitalWrite(GH1_V, GH[1]);
        digitalWrite(GH2_V, GH[2]);
        digitalWrite(GH3_V, GH[3]);
        digitalWrite(GH4_V, GH[4]);
        digitalWrite(MT_S, MT);
        wateringCount_th = 0; //이 변수는 자동 관수 횟수에 대한 계산임. 나중에 필요할 때 불러 쓸 필요가 있어서 만들어 놓은 것임.
    }
    // 자동 모드 - millis()를 기준은 현재 관수 위치를 계속 계산해 내야되는 어려움이 있다.
    else {  // 자동 모드   
        //안전 판별 기준, (관수회수의 합*1회당 관수시간)과 관수주기를 비교. 작을때만 의미 있고 클때는 처음부터 다시 
        if(wateringTimeSec*((int)GH[1]+(int)GH[2]+(int)GH[3]+(int)GH[4]) > periodMin*60) {      
        return;
        }
        // (자동모드이며) 관수 주기, 개별관수 시간 그리고 관수 동수가 조건에 일치할 때만 다음의 조건식 안으로 들어간다.
        if((unsigned long)(millis()-wateringStart) <= (unsigned long)((unsigned long)periodMin * (unsigned long)60000)){
        //debug
        //Serial.print("millis()-wateringStart : periodMin\t"); Serial.print (millis()-wateringStart); 
        //Serial.print (" :-> compared value "); Serial.println( (unsigned long)(periodMin * 60000));
        //whatGH[]와 (int)NoGH를 이용해서 루프 관수를 시작하는 process. 이 배열과 변수는 BLYNK_WRITE(V7)에 정의되어 있다.
        int argWhatGH;
        argWhatGH = (int)((millis()-wateringStart)/((unsigned long)wateringTimeSec*(unsigned long)1000))+1;
        //계산한 값이 전체 관수해야할 온실 총수를 초과할 경우 관수는 종료하고 관수 주기만큼 기다린다.
        if(argWhatGH <= NoGH) { 
            wateringThisGH(whatGH[argWhatGH]);
            MotorOn();
            //debug
            //Serial.print("now watering : "); Serial.println(whatGH[argWhatGH]);
        }
        else{ // 관수를 종료하고 관수주기만큼 기다린다.
            wateringThisGH(0);
            MotorOff();
            //debug
            //Serial.print("now watering : "); Serial.print("stop"); Serial.print("\t\twateringStart millisecond: "); Serial.println(wateringStart);
        }   
        //관수 주기를 초과하면 다시 wateringStart를 초기화하고 loop()를 다시 시작한다.
        
        } 
        else { // 관수주기와 관수회수의 합을 비교. 클때 생기는 조건이다. == 새로운 초기화 조건실행
        wateringStart = millis();
        //debug 
        //Serial.print("wateringStart = "); Serial.print(wateringStart); Serial.print("millis() : "); Serial.println(wateringStart);
        wateringCount_th++;
        }
    }
}