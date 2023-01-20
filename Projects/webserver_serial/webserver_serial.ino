#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

char *ssid = "KT_WiFi_2G_F18E";
char *password = "5ag15dz085";

ESP8266WebServer server(80);
int LED_pin = 2;        // LED 연결 핀
boolean LED_state = false;

DHT dht(0, DHT11);
void setup(void) {
  Serial.begin(9600);

  Serial.print("Connecting to \'");
  Serial.print(ssid);
  Serial.println("\'");

  WiFi.begin(ssid, password);   // AP에 접속

  // AP에 접속이 될 때까지 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected.");

  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());   // 서버에 할당된 주소

  // 처리 함수 등록
  server.on("/", handleRoot);
  server.on("/led.cgi", handleONOFF);

  server.begin();       // 웹 서버 시작
  Serial.println("HTTP server started");

  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, LED_state);
}

void loop(void) {
  server.handleClient();      // 클라이언트 요청 처리

  delay(1000);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();


  if (isnan(temp) || isnan(hum)) {
    Serial.println("**Error in reading DHT sensor!");
    return;
  }
  Serial.print("Temperature :");
  Serial.print(temp);
  Serial.println("C ");
  Serial.print("Humidity :");
  Serial.print(hum);
  Serial.println("%");

  HTTPClient httpClient;

  String host = "http://api.thingspeak.com";
  String url = "/update?key=";
  String key = "SGE75DH6HSE2LRRJ";
  String field1 = "&field1=";
  String field2 = "&field2=";

  httpClient.begin(host + url + key + field1 + temp + field2 + hum);

  int httpCode = httpClient.GET();

  //접속에 오류가 발생하면 음의 값을 반환
  if (httpCode >0){
    Serial.printf("[HTTP] Temperature & Humidity data was logged...\n");
  }
  else{
    Serial.printf("[HTTP] Connection failed : %s\n", httpClient.errorToString(httpCode).c_str());
  }

  httpClient.end();

  delay(30000);
}

void handleRoot() {
  String message = "";
  message += "<html>";
  message += "<body>";

  message += "Currently... LED is ";    // 현태 LED 상태 표시
  message += (LED_state ? "ON." : "OFF.");

  // CGI를 통해 사용자 입력
  // ‘Submit’ 버튼을 누르면
  // ‘/led.cgi’ 페이지로 ‘LEDstatus’ 인자와 함께 요청이 이루어짐
  message += "<br />";
  message += "<FORM method=\"get\" action=\"/led.cgi\">";
  message += "<P><INPUT type=\"radio\" name=\"LEDstatus\" value=\"1\">Turn ON";
  message += "<P><INPUT type=\"radio\" name=\"LEDstatus\" value=\"0\">Turn OFF";
  message += "<P><INPUT type=\"submit\" value=\"Submit\"> </FORM>";

  message += "</body>";
  message += "</html>";

  server.send(200, "text/html", message);   // HTML 형식 페이지 전송
}

void handleONOFF() {        // LED ON & OFF 페이지
  if (server.argName(0) == "LEDstatus") { // 인자 이름
    int state = server.arg(0).toInt();    // 인자의 값

    LED_state = state;
    digitalWrite(LED_pin, LED_state);   // LED 상태 반영
    //add 정완
    Serial.print(LED_pin); Serial.println(LED_state);
    String message = "";
    // HTML 내용
    message += "<html>";
    message += "<body>";
    message += "Now... LED is ";      // LED 상태 표시
    message += (LED_state ? "ON." : "OFF.");
    message += "<br /><br />";

    // 메인 페이지로의 링크 표시
    message += "<a href=\"/\">Main Control Page</a>";

    message += "</body>";
    message += "</html>";

    server.send(200, "text/html", message); // HTML 형식 페이지 전송
  }
}
