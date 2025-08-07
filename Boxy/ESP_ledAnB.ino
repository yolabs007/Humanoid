/*
This code works on one- way BLE comunication 
You do not require any other peripheral except a code - BLE_senderAnB.py running in your laptop 
1. Step 1- Upload the code into ESP32 
2. Run the python code in your computer 
3. send a and b command from your python code input terminal 
4. see the light going on and off 
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer*         pServer         = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool               deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* /*srv*/)    { deviceConnected = true;  }
  void onDisconnect(BLEServer* /*srv*/) { deviceConnected = false; }
};


void setup() {
  Serial.begin(115200);
  delay(100);

  // Attach servo at 50 Hz (standard) with pulse range 1000-2000 µs

 pinMode(2,OUTPUT);
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
      digitalWrite(2,HIGH);
      }
    
    else if (c == 'b') {
      Serial.println("Command B executed");
       digitalWrite(2,LOW);
   
      
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

