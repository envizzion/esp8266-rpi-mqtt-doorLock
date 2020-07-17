


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>


// Update these with values suitable for your network.

const char* ssid = "SLT-ADSL-17022";
const char* password = "ran2280252";
const char* mqtt_server = "192.168.1.10";

const char* device_id = "lock_01"; //this will be the subscribe topic also
const char* subs_topic = "lock_01"; 
const char* pub_topic = "raspberry";
String pub_msg = "";


//wifi and mqtt related variables and objects
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//rfid related variables and objects
#define SS_PIN 5  //D2
#define RST_PIN 4 //D1
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

String content = "";
String id = "";


#define WIFI_LED    D0  //GREEN 
#define ALERT_PIN  D3 //RED;
#define BUZZER_PIN   D4  //YELLOW
#define BUTTON A0 



byte wifi_status=0;



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkWifi();
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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
         setLock(1);
  } else {
        setLock(0);
  }

}

void reconnect() {
  // Loop until we're reconnected


  
 if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);

 }

  while (WiFi.status() != WL_CONNECTED){
      blinkWifi();
      delay(500);
 }
  setWifiLed(0);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(device_id)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     
      // ... and resubscribe
      client.subscribe(subs_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
     
      delay(3000);
    }
  }
  setWifiLed(1);
  
}


void blinkWifi(){
    wifi_status = !wifi_status;
  digitalWrite(WIFI_LED,wifi_status);
  }

void setWifiLed(byte state){
  wifi_status  = state ;
  digitalWrite(WIFI_LED,wifi_status);
 } 

// 1 = lock the door , 0 - unlock the door
//also controls the buzzer and the _ALERT_LED
void setLock(byte state){
       
       digitalWrite(ALERT_PIN,state);

       //BUZZER to do 
    
    }

  
  

 //Function to read RFID card information
boolean readCard() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }

  content = "";
  id = "";
              byte letter;
              for (byte i = 0; i < mfrc522.uid.size; i++)
              {
              content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
              content.concat(String(mfrc522.uid.uidByte[i], HEX));
            }

              id = content.substring(1);

              Serial.println(id);
               return true;
}


void publishRFIDData(String id){


    pub_msg = String(device_id);
    pub_msg+=",rfid,";
    pub_msg+=String(id);

    char apiArray[pub_msg.length() + 1];
    pub_msg.toCharArray(apiArray,pub_msg.length() + 1);

    
    client.publish(pub_topic, apiArray);
  
  }


void setup() {
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  pinMode(WIFI_LED,OUTPUT);
  pinMode(ALERT_PIN,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
 //rfid spi communication initialization
   SPI.begin();      // Initiate  SPI bus
   mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  
//
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
//    ++value;
//    snprintf (msg, 50, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("outTopic", msg);
  //here the card information is read if there is any detected
if(readCard()){


  publishRFIDData(id);
   while(readCard())delay(1000);
  }
  }
}
