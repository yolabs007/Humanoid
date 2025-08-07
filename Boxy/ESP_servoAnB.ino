/*
This code works on one- way BLE comunication 
You will require a servo connected to ESP32...... 
Note : check the version of servo library version  is working by running a standard servo sweep code 

1. Step 1- Upload the code into ESP32 
2. Servo Connections 
   
   ESP           Servo 

  Vin -          Servo power ( middle wire)
  GND -          GND - Dark brown or black wire  
  D4  -          Signal - Yellow/Orange Wire 

2. Run the python code in your computer 
3. send a and b command from your python code input terminal 
4. Observ the servo changing its position 
This code is written by - Rahul Sharma for Yolabs on - 7th Aug'25 
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer*         pServer         = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool               deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* /*srv*/)    { deviceConnected = true;  }
  void onDisconnect(BLEServer* /*srv*/) { deviceConnected = false; }
};

Servo myservo;              // servo object
int  servoPin     = 4;      // GPIO pin connected to servo signal
int  pos          = 0;      // variable to store servo position

void setup() {
  Serial.begin(115200);
  delay(100);

  // Attach servo at 50 Hz (standard) with pulse range 1000-2000 µs
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 1000, 2000);
  myservo.write(90);  // start at mid position

  // Initialize BLE
  BLEDevice::init("ESP32_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create BLE service and characteristic
  BLEService* service = pServer->createService(SERVICE_UUID);
  pCharacteristic = service->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  service->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE Peripheral started, waiting for connection...");
}

void loop() {
  if (!deviceConnected) {
    delay(100);
    return;
  }

  String rx = pCharacteristic->getValue();
  if (rx.length() > 0) {
    char c = rx[0];
    Serial.print("Received: ");
    Serial.println(c);

    if (c == 'a') {
      Serial.println("Command A executed");
      for (pos = 0; pos <= 180; pos++) {
        myservo.write(pos);
        delay(15);
      }
    }
    else if (c == 'b') {
      Serial.println("Command B executed");
      for (pos = 180; pos >= 0; pos--) {
        myservo.write(pos);
        delay(15);
      }
    }

    // Optionally notify back to client
    String reply = String("Done: ") + c;
    pCharacteristic->setValue(reply);
    pCharacteristic->notify();

    // Clear to detect next write
    pCharacteristic->setValue("");
  }

  delay(100);
}



