/*************************************************************************
 * 이 프로그램은 3동 LED 제어 프로그램이다.
 * 프로그램의 원천은 ESP01_MQTT_compressor에서 왔다.
 * 원격에서 "topic" : Actuator/GH3/LED/Lightall에다 on/off를 전환한다.
 * "LastWill" 메세지에서 클라이언트의 상태를 표시한다. 양쪽다 retain으로 해야한다.
 * ESP01에 물린 릴레이는 타이머와 병렬로 이루어져 있다. 켤 수는 있으나
 *      상대편의 상태를 off할 수는 없다. ((나중에 릴레이를 하나 더 달아서 해결해야 하겠다.))
**************************************************************************/
// 현재 프로그램의 개발 단계 TEST
#define TEST

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_PIN 2
#define LED_LIGHT_PIN 0 //esp01 relay module에서 모터 신호는 negative signal

#ifdef TEST //프로그램 개발하는 곳의 환경 요건
    #define ssid        "FarmMain5G"
    #define password    "wweerrtt"
    #define mqtt_server "192.168.0.24"
#elif //실제 농장에서의 implimentation
    // Update these with values suitable for your network.
    #define ssid        "FarmMain5G"
    #define password    "wweerrtt"
    #define mqtt_server "192.168.0.24"
#endif

#define LASTWILL    "Lastwill/GH3/LED/Lightall"
#define LEDLIGHT    "Actuator/GH3/LED/Lightall"
#define MQTTID      "GH3_LIGHT_MQTT"
#define MQTTUSER    "farmmain"
#define MQTTPASS    "eerrtt"

char message[256]; //MQTT message를 여기에 복사해 놓고 쓰겠다.
int motor;  //motor의 상태를 받아온다. 1이면 motorOn(), 0이면 motorOff()

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

void lightOn(void) {
    digitalWrite(LED_LIGHT_PIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
}

void lightOff(void) {
    digitalWrite(LED_LIGHT_PIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
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
    if(strcmp(topic, LEDLIGHT) == 0) {
        motor = atoi(message);
        if (motor) lightOn();
        else lightOff();
         //debug
        //Serial.printf("the content of message is %s\n", message);
        //Serial.printf("int value of dura is : %i\n", dura);
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(MQTTID, MQTTUSER, MQTTPASS, LASTWILL, 0, 1, "offline")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.subscribe(LEDLIGHT);
            client.publish(LASTWILL, "online", 1);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    pinMode(LED_LIGHT_PIN, OUTPUT);
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
    delay(500);
    // loop()안에서 해결해야할 코드는 아래에 입력
}