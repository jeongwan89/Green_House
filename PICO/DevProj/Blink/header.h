#include <WiFiEspAT.h>
#include <PubSubClient.h>

char ssid[] = "FarmMain5G";
char pass[] = "wweerrtt";
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 0, 24);

WiFiClient espClient; // WiFiEspAT에서 정의됨
PubSubClient client(espClient);

//----------------------------------------------------------------------
// definition
//----------------------------------------------------------------------
// resetFunction
void resetFunc(void)
// 리셋함수는 어드레스가 0부터 시작하는 함수이다.
{
    watchdog_enable(1, 1);
    while (1);
}

/* setup() 안에서 작동한다. 이전에 반드시 정의 되어야할 것이 있는데,
    ESP01이 SoftwareSerial로 정의되어 있어야 하고(EspSerial),
    WiFi 쉴드 변수와 ESP01모뎀의 객체의 레퍼런스를 연결해 주어야 한다.
*/
void wifiConnect(void)
{
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("WiFi shield not present");
        delay(100);
        resetFunc();
    }

    Serial.print("Attempting to connect to WPA SSID:");
    Serial.println(ssid);
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        WiFi.begin(ssid, pass);
        if (cnt > 50)
            resetFunc();
        cnt++;
    }

    Serial.println("\nYour're connected to the networtk");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
