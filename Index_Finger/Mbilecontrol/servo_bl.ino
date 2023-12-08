/* Control the Servo Using a Mobile App
  Sweep a servo  from Zero to 180. Connect to any digital(PWM) pin and servo will rotate. 
  Refer the diagram  for connection to servo  and ESP32
  Connections 
  Servo  -            ESP32 
  Brown -             Ground
  Red   -             3v3  // like sensors do not try to connect it to a digital pin by making it high- you will not the required current
  Orange/Yellow  -    Digital pin - D4 in this code 
  
  NOTE : If your servo need to take a good load or if you are connecting multiple servo,  
  you will  need external power supply with/without servo controller board               
  
  by Rahul Sharma for Yolabs 
  This example code is in the public domain.
  https://www.yolabs.in
*/

#include <Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
Servo myservo;

void setup() {
  myservo.attach(4);  // attaches the servo on pin 13
  SerialBT.begin("ESP32Test_index"); // Bluetooth device name
}

void loop() {
  if (SerialBT.available()) {
    String received = SerialBT.readStringUntil('\n');  // very imp otherwise you will find the servo is legggy and received string arbitery 
    int pos = received.toInt();
    myservo.write(pos);
  }
}
