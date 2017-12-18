import paho.mqtt.client as mqtt

client = mqtt.Client()
client.connect("iot.franciscocalaca.com", 1883, 60)
client.publish("leandro/acionamento",'{"status": 0}')