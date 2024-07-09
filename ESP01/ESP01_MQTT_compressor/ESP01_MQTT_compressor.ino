/*************************************************************************
 * blynk server가 폐쇄되어서 MQTT로 이전하는 프로그램
 * farmmain.iptime.org:1883
 * 192.168.0.24:1883
 * MCU는 ESP01 + 릴레이로 연결되어 있는 것으로 한다. 
 *      GPIO0가 릴레이 작동 핀이다.
**************************************************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_PIN 2
#define MOTOR_PIN 0 //esp01 relay module에서 모터 신호는 negative signal

// Update these with values suitable for your network.
const char* ssid = "FarmMain5G";
const char* password = "wweerrtt";
const char* mqtt_server = "192.168.0.24";

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

void motorOn(void) {
    digitalWrite(MOTOR_PIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
}

void motorOff(void) {
    digitalWrite(MOTOR_PIN, HIGH);
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
    if(strcmp(topic, "Actuator/FM/comp/motor") == 0) {
        motor = atoi(message);
        if (motor) motorOn();
        else motorOff();
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
        if (client.connect("ESP8266_FM_comp", "farmmain", "eerrtt",
                            "Lastwill/FM/comp/status", 0, 1, "offline")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.subscribe("Actuator/FM/comp/motor");
            client.publish("Lastwill/FM/comp/status", "online", 1);
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
    delay(500);
    // loop()안에서 해결해야할 코드는 아래에 입력
}