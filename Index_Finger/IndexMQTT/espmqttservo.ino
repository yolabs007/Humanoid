/* Move the INDEX finger 
  This code is written to check if the index finger assembly is working to its perfection. 
  
  Connections 
  Servo  -            ESP32 
  Brown -             Ground
  Red   -             3v3
  Orange/Yellow  -    Digital pin - D4 in this code 
  
  NOTE : If your servo need to take a good load or if you are connecting multiple servo,  
  you will  need external power supply with/without servo controller board               
  
  by Rahul Sharma for Yolabs 
  This example code is in the public domain. Noe for Servo Control please use ServoESP32 library 
  https://www.yolabs.in
  updated on - 10th march 2024 
Note: ESP32 are not known for their wifi range so keep esp close to HOTSPOT 
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

const char* ssid = "rahul2g";     // change with ur SSID
const char* password = "123456789"; //change with your password try to keep it simple  and 9 digits to avoid mistake 
const char* mqtt_server = "mqtt.eclipseprojects.io";

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo_2;

int atrest_angle = 0;    // Angle for state 3 (rest)
int atmid_angle = 70;    // Angle for state 2 (mid)
int atclose_angle = 140; // Angle for state 1 (close)

void setup_wifi() {
  delay(10);
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
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Take action based on the received message
  if (strncmp((char*)payload, "1", length) == 0) {
    servo_2.write(atclose_angle); // Close the servo
  } else if (strncmp((char*)payload, "2", length) == 0) {
    servo_2.write(atmid_angle); // Move the servo to mid position
  } else if (strncmp((char*)payload, "3", length) == 0) {
    servo_2.write(atrest_angle); // Move the servo to rest position
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("your/topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  servo_2.attach(4); // Attach servo to pin 4
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
