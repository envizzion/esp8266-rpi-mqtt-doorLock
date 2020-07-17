import paho.mqtt.client as mqtt



# dictionary with allowed Ids

class Lock:
 def __init__(self, id ,rfid_key , state):
    self.id = id
    self.rfid_key = rfid_key
    self.state = state

locks =	{
  "lock_01": Lock("lock_01","02 ef dd 34",0),
  "lock_02": Lock("lock_02","02 ef dd 35",0),
}


# configurations

client = mqtt.Client()
client.connect(host="localhost",port = 1883,keepalive=60)


# This is the Publisher
# client.publish("test", "Hello world!");



# This is the Subscriber

def setLock(lock_id ,rfid_key ):

    if lock_id in locks:
      lock = locks[lock_id];
      if(lock.rfid_key==rfid_key):
          print("key matches")
          lock.state = not lock.state 
          client.publish(lock_id, "1" if lock.state == 1 else  "0");
   
    for lock in locks.values():
        print (lock.id +","+ lock.rfid_key +","+ lock.state)

def on_connect(client, userdata, flags, rc):
  print("Connected with result code "+str(rc))
  client.subscribe("raspberry")

def on_message(client, userdata, msg):
 
  print(msg.payload.decode())
  lock_id,msgType,rfid_key = str(msg.payload.decode()).split(",");     
  setLock
  
  setLock(lock_id ,rfid_key)


client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()




client.disconnect();