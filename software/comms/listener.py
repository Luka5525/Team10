#!/usr/bin/env python
import rospy
import paho.mqtt.client as mqtt
from std_msgs.msg import String

def on_message(client, userdata, msg):
   command_received_from_microcontroller=true
   return command_received_from_microcontroller

client = mqtt.Client()    
client.on_message = on_message
client.connect(192.168.43.221, 1883, 60)

if __name__ == '__main__':
    listener()