#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define mqtt_devid ""
#define mqtt_proid ""
#define mqtt_apkey ""

// Update these with values suitable for your network.

const char* ssid = "WIN";
const char* password = "66666666";
const char* mqtt_server = "183.230.40.39";
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg_buf[200];
int value = 0;  //
char dataTemplete[]="{\"01temp\":%d,\"01Humi\":%d}";
char msgJson[75];
char debug_buf[200];
int i;
unsigned short json_len=0;
uint8_t debug_buffer_start_index = 0;
 
int pl_0=0;
byte temp=0;
byte humi=0;
int temp_r = 0;
int humi_r = 0;
 
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
  }
 
  randomSeed(micros());
 
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

  } else {
    
  }
 
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_devid,mqtt_proid,mqtt_apkey)) {  //One net user name as product ID , and password as APIKey
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 6002);  //not 1883 , one net use the port of 6002 as mqtt server
  client.connect(mqtt_devid,mqtt_proid,mqtt_apkey);
  client.setCallback(callback);
}
 
void loop() {
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  
  if (now - lastMsg > 10000) {
    lastMsg = now;
    //++value;
    temp=0;
    humi=0;
    
    temp_r = random(-20, 40);
    humi_r = random(1, 80);
    snprintf(msgJson, 40, dataTemplete, temp_r, humi_r);
    memset(msg_buf,0,200);
    json_len=strlen(msgJson); //packet length count the end char '\0'
    msg_buf[0]=char(0x03);  //palyLoad packet byte 1, one_net mqtt Publish packet payload byte 1, type3 , json type2
    msg_buf[1]=char(json_len>>8);  //high 8 bits of json_len (16bits as short int type)
    msg_buf[2]=char(json_len&0xff); //low 8 bits of json_len (16bits as short int type)
    memcpy(msg_buf+3,msgJson,strlen(msgJson));
    Serial.print("Publish message: ");
    Serial.println(msgJson);
    client.publish("$dp",(uint8_t*)msg_buf,3+strlen(msgJson),false); // msg_buf as payload length which may have a "0x00"byte
  }
  
}
