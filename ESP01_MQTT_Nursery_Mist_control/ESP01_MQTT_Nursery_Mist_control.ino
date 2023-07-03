/*************************************************************************
 * blynk server가 폐쇄되어서 MQTT로 이전하는 프로그램
 * farmmain.iptime.org:1883
 * 192.168.0.24:1883
 * MCU는 ESP01 + 릴레이로 연결되어 있는 것으로 한다.
**************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED_PIN 2
#define MOTOR_PIN 0

// Update these with values suitable for your network.
const char* ssid = "FarmMain5G";
const char* password = "wweerrtt";
const char* mqtt_server = "192.168.0.24";

char message[256]; //MQTT message를 여기에 복사해 놓고 쓰겠다.
int dura;   //1회당 관수 시간 (초)
int peri;   //관수 주기(분) mqtt period에서 따온다
int motor;  //수동일 때 모터 기동
int isAuto; //자동 관수이냐?
long lastwatering; // 마지막 관수한 시간이 필요하다. millis()값을 저장할 것임
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    }

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        message[i] = payload[i];
        message[i+1] = NULL;
    }
    Serial.println();
    //topic의 내용을 분석하고 그에 알맞은 처리를 담당한다.
    if(strcmp(topic, "Argument/NR/mist/dura") == 0) {
        dura = atoi(message);
        //debug
        //Serial.printf("the content of message is %s\n", message);
        //Serial.printf("int value of dura is : %i\n", dura);
    } else if(strcmp(topic, "Argument/NR/mist/period") == 0) {
        peri = atoi(message);
        //debug
        //Serial.printf("int value of period is : %i\n", peri);
    } else if(strcmp(topic, "Argument/NR/mist/motor") == 0) {
        motor = atoi(message);
        //debug
        //Serial.printf("int value of motor is : %i\n", motor);
    } else if(strcmp(topic, "Argument/NR/mist/auto") == 0) {
        isAuto = atoi(message);
        if (isAuto) lastwatering = millis();
        //debug
        //Serial.printf("int value of isAuto is : %i\n", isAuto);
    }

}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266_NR_mistcontrol", "farmmain", "eerrtt",
                        "Lastwill/NR/mist/status", 0, 1, "offline")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.subscribe("Argument/NR/mist/dura");
            client.subscribe("Argument/NR/mist/period");
            client.subscribe("Argument/NR/mist/motor");
            client.subscribe("Argument/NR/mist/auto");
            client.publish("Lastwill/NR/mist/status", "online", 1);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void motorOn(void){ //esp01 relay 모듈을 쓸 때, 신호가 부신호로 잡히기 때문에 
    digitalWrite(MOTOR_PIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
}

void motorOff(void){
    digitalWrite(MOTOR_PIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(MOTOR_PIN, OUTPUT);
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    long now = millis();

    //자동인가?
    if (isAuto == 1) {
        //관수 주기 안에 있나?
        if ((now - lastwatering) <= (peri * 60000)) {
            if ((now - lastwatering) <= (dura * 1000)) {
                motorOn();
            } 
            else {
                motorOff();
            }
        } 
        else {
            lastwatering = millis();
        }
    }
    //자동이 아니다 => 수동이다.
    else {
        if (motor == 1) {
            motorOn();
        }
        else {
            motorOff();
        }
    }
}