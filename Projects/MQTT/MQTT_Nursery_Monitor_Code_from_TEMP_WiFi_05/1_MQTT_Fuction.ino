void init_MQTT_ESP01()
{
  WiFi.init(&ESPSerial);
  // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
    // don't continue
    while (true);
    }
  // attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(WIFI_SSID);
       // Connect to WPA/WPA2 network
       status = WiFi.begin(WIFI_SSID, WIFI_PW);
    }
  // you're connected now, so print out the data
    Serial.println("You're connected to the network"); 
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
    Serial.print("MAC address = ");
    // Display MAC Address
        WiFi.macAddress(mac);
        Serial.print(mac[5],HEX);
        Serial.print(":");
        Serial.print(mac[4],HEX);
        Serial.print(":");
        Serial.print(mac[3],HEX);
        Serial.print(":");
        Serial.print(mac[2],HEX);
        Serial.print(":");
        Serial.print(mac[1],HEX);
        Serial.print(":");
        Serial.println(mac[0],HEX);

        mqtt_client.setServer(MQTT_BROKER_ADDR, MQTT_BROKER_PORT);
}

void establish_mqtt_connection(){
  if(mqtt_client.connected()) 
  
    //LCD 연결 성공
    lcd.clear(); lcd.setCursor(0,0); lcd.print("mqtt_client->connected");
    
    return;
  while(!mqtt_client.connected()){
    Serial.println("Try to connect MQTT Broker");
    if(mqtt_client.connect(MQTT_ID, MQTT_UR, MQTT_PW)){
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqtt_client.state());
      
      //LCD 에러 출력부분
      lcd.clear(); lcd.setCursor(0,0); lcd.print("failed, rc="); lcd.print(mqtt_client.state());
      
      delay(2000);
    }
  }
}
