#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// Track connection status
enum { PLACEHOLDER };
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer*) override    { deviceConnected = true; }
  void onDisconnect(BLEServer*) override { deviceConnected = false; }
};

Servo myservo;
const int servoPin = 4;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize servo
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 1000, 2000);
  myservo.write(90);

  // Initialize BLE
  BLEDevice::init("ESP32_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create service and write-only characteristic
  BLEService* service = pServer->createService(SERVICE_UUID);
  pCharacteristic = service->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->addDescriptor(new BLE2902());

  service->start();
  BLEAdvertising* advertising = pServer->getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE Peripheral started");
}

void loop() {
  if (deviceConnected) {
    String rx = pCharacteristic->getValue();
    if (rx.length() > 0) {
      int angle = rx.toInt();
      angle = constrain(angle, 0, 180);
      myservo.write(angle);
      Serial.printf("Set angle: %d\n", angle);
      pCharacteristic->setValue("");
    }
  }
  delay(100);
}
