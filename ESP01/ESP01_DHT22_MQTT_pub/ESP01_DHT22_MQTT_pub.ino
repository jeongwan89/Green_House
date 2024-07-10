#include "source.h"

void setup()
{
    Serial.begin(115200);
    // =====================================================================================
    // Optional functionalities of EspMQTTClient
    // Enable debugging messages sent to serial output
    client.enableDebuggingMessages();

    // Enable the web updater.
    // User and password default to values of MQTTUsername and MQTTPassword.
    // These can be overridded with enableHTTPWebUpdater("user", "password").
    client.enableHTTPWebUpdater();

    // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword.
    // Port is the default OTA port. Can be overridden with enableOTA("password", port).
    client.enableOTA();

    // You can activate the retain flag by setting the third parameter to true
    client.enableLastWillMessage(WILLTOPIC, "offline", 1);
    // =====================================================================================
    // Initialize device.
    dht.begin();
    Serial.println(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print(F("Sensor Type: "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:  "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:   "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:   "));
    Serial.printf("%.2f",sensor.max_value);
    Serial.println(F(" C"));
    Serial.print(F("Min Value:   "));
    Serial.print(sensor.min_value); Serial.print("  ");
    Serial.println(F(" C"));
    Serial.print(F("Resolution:  "));
    Serial.print(sensor.resolution);
    Serial.println(F(" C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print(F("Sensor Type: "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:  "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:   "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:   "));
    Serial.print(sensor.max_value);
    Serial.println(F(" %"));
    Serial.print(F("Min Value:   "));
    Serial.print(sensor.min_value);
    Serial.println(F(" %"));
    Serial.print(F("Resolution:  "));
    Serial.print(sensor.resolution);
    Serial.println(F(" %"));
    Serial.println(F("------------------------------------"));
    // Set delay between sensor readings based on sensor details.
    delayMS = sensor.min_delay / 1000;
    // =====================================================================================
}

void loop()
{
    client.loop();
    // Delay between measurements.
    delay(delayMS);
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        Serial.println(F("Error reading temperature!"));
    }
    else
    {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F(" C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println(F("Error reading humidity!"));
    }
    else
    {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F(" %"));
    }
}