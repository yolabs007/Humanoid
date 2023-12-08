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
  This example code is in the public domain.
  https://www.yolabs.in
  updated on - 8th dec 2023 
*/

#include <Servo.h>

Servo servo_2;  //create a servo object

int angle = 0;   // servo position in degrees 

int atrest_angle = 0;
int atmid_angle = 70;
int atclose_angle = 140;

 

void setup()
{
  servo_2.attach(4);   //attach a pin to servo 

}


void loop() 
{ 
                            
    servo_2.write(atrest_angle);               
    delay(3000);
    servo_2.write(atmid_angle);               
    delay(3000);
    servo_2.write(atclose_angle);               
    delay(3000);
} 
