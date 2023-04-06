import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Publish a message to the topic
    client.publish("your/topic","hello")

# Create a MQTT client instance
client = mqtt.Client()
client.on_connect = on_connect

# Set your broker address and port number
client.connect("mqtt.eclipseprojects.io", 1883, 60)
counter = 0
for i in range(10):
  counter = counter+1
  client.publish("your/topic","hello"+str(counter))
  time.sleep(1)
# Blocking call that processes network traffic, dispatches callbacks, and handles reconnecting
client.loop_forever()
