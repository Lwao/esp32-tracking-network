#include <WiFi.h>
#include <PubSubClient.h>

#define RSSI_REF1 -43.5225 
#define RSSI_REF2 -26.3252
#define RSSI_REF3 -29.7935
#define PATH_LOSS 2

// AP configuration ID
const char* ssid_own = "REF0_node";
const char* pwd_own  = "00000000";

// Local NET configuration ID
const char* ssid_net = "your_network_ssid";
const char* pwd_net  = "your_network_pwd";

// MQTT configuration
const char* brokerUser = "mqtt";
const char* brokerPass = "password";
const char* broker = "192.168.0.6";
unsigned mqtt_port = 1883;

// out topics to publish and in topics to subscribe
const char* outTopic ="/RSSI_vector";
const char* inTopic1 ="/RSSI_1";
const char* inTopic2 ="/RSSI_2";
const char* inTopic3 ="/RSSI_3";

// AP fixes nodes variables 
const char *ssid_AP1 = "REF1_node";
const char *ssid_AP2 = "REF2_node";
const char *ssid_AP3 = "REF3_node"; 

// Initialize Wi-Fi AP server and MQTT client
WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

//float d_AP1, d_AP2, d_AP3; // unidirectional distances
long rssi_FXD1, rssi_FXD2, rssi_FXD3; // RSSI readings in MVB_ND_1 node
long rssi_MVB1=0, rssi_MVB2=0, rssi_MVB3=0; // RSSI readings in each FXD_ND_X node

/*
void track_unidirectional_distance(){ // unidirectional readings there are no used, but still implemented for tests
  // d = pow(10, (RSSI-RSSI_REF)/(-10*PATH_LOSS));
  // rx = dAP1^2/(2*d)-(dAP2^2-d^2)/(2*d);
  // ry = (dAP1^2)/(2*sqrt(3)*d)+(dAP2^2-d^2)/(2*sqrt(3)*d)-2*(dAP3^2-d^2)/(2*sqrt(3)*d);

  d_AP1 = pow(10, (rssi_FXD1-RSSI_REF1)/(-10*PATH_LOSS));
  d_AP2 = pow(10, (rssi_FXD2-RSSI_REF1)/(-10*PATH_LOSS));
  d_AP3 = pow(10, (rssi_FXD3-RSSI_REF1)/(-10*PATH_LOSS));
}
*/

void scan_fxd_rssi(){ // scan wi-fi networks for rssi of fixed nodes
  char plot[64];
	int nNet = WiFi.scanNetworks(); // number of wifi networks
    if (nNet == 0) {
        Serial.println("NULL");
    } else {
        for (int i = 0; i < nNet; ++i) {
            // Print SSID and RSSI for specific fixed AP nodes 
            boolean flag_AP1 = WiFi.SSID(i).equals(ssid_AP1);
            boolean flag_AP2 = WiFi.SSID(i).equals(ssid_AP2);
            boolean flag_AP3 = WiFi.SSID(i).equals(ssid_AP3);
            boolean test_RSSI = flag_AP1 || flag_AP2 || flag_AP3; // track only the fixed nodes

            // store fixed nodes rssi
      			if(flag_AP1){rssi_FXD1 = (long) WiFi.RSSI(i);}
      			if(flag_AP2){rssi_FXD2 = (long) WiFi.RSSI(i);}
      			if(flag_AP3){rssi_FXD3 = (long) WiFi.RSSI(i);}    
        }
    }
    // Plot last RSSI readings in movable node
    sprintf(plot, "(%d,%d,%d)", rssi_FXD1, rssi_FXD2, rssi_FXD1);
    Serial.print("Scanned RSSI: ");
    Serial.println(plot); 
    delay(10);
}

void init_AP(){ // initialize as access point mode
  // Initialize AP
  WiFi.softAP(ssid_own, pwd_own);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
  server.begin();
}

void init_STA(){ // initialize as station mode
  // Initialize STA
  WiFi.begin(ssid_net, pwd_net);
  Serial.print("Connecting to "); 
  Serial.println(ssid_net);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) //retry connection if failed
  {
    delay(10);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Node local IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){ // reconnect to broker and subscribe to inTopics
  while(!client.connected()){
    Serial.print("Connecting to ");
    Serial.println(broker);
    if(client.connect("REF0", brokerUser, brokerPass)){
      Serial.print("Connected to ");
      Serial.println(broker);
      client.subscribe(inTopic1);
	  client.subscribe(inTopic2);
	  client.subscribe(inTopic3);
    } else { // retry connecting if falied
      Serial.println("\n Trying to reconnect");
      delay(100);
    }
  }
}

long byteTolong(byte* payload, unsigned int length){ // convert received byte string in long int
  long result=0;

  if(length==2){
      //result = -1 * (long)payload[2];
      result = (long) ((char) payload[1] - '0')*1;
  } else if(length==3){
      //result = -1 * ((long)payload[2]*10 + (long)payload[3]);
      result = (long)( ((char)payload[1] - '0')*10 + ((char) payload[2] - '0')*1);
  } else if(length==4){
      //result = -1 * ((long)payload[3]*100 + (long)payload[4]*10 + (long)payload[5]);
      result = (long)(((char)payload[1] - '0')*100 + ((char)payload[2] - '0')*10 + ((char) payload[3] - '0')*1);
  }
  return -1*result;
}

void callback(char* topic, byte* payload, unsigned int length){ // callback for topics that this device is subscribe to
	if (strcmp(topic, inTopic1) == 0)	// callback for inTopic 1 to retrieve RSS1 from FXD_ND_1
	{
		Serial.print("T1: ");
    Serial.print(length);
		Serial.print(topic);
		for(int i=0; i<length; i++){
			Serial.print((char) payload[i]);
		}
    rssi_MVB1 = byteTolong(payload, length);
		Serial.println();
	}
	if (strcmp(topic, inTopic2) == 0)	 // callback for inTopic 2 to retrieve RSS1 from FXD_ND_2
	{
		Serial.print("T2: ");
    Serial.print(length);
		Serial.print(topic);
		for(int i=0; i<length; i++){
			Serial.print((char) payload[i]);
		}
    rssi_MVB2 = byteTolong(payload, length);
		Serial.println();
	}
	if (strcmp(topic, inTopic3) == 0) // callback for inTopic 3 to retrieve RSS1 from FXD_ND_3	
	{
		Serial.print("T3: ");
    Serial.print(length);
		Serial.print(topic);
		for(int i=0; i<length; i++){
			Serial.print((char) payload[i]);
		}
    rssi_MVB3 = byteTolong(payload, length);
		Serial.println();
	}  
}
 

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_AP_STA);
  init_AP();
  init_STA();
  
  client.setServer(broker, mqtt_port);
  client.setCallback(callback);
  
  delay(10);
}

void loop() {
  char plot[64];
  char messages[64];
  
  scan_fxd_rssi();
  //track_unidirectional_distance(); 
  if (!client.connected()){ // check connection to broker
      reconnect();
    }
  client.loop();
  
  // Publish message to outTopic
  sprintf(messages, "(%f,%f,%f)", (float)(rssi_FXD1+rssi_MVB1)/2, (float)(rssi_FXD2+rssi_MVB2)/2, (float)(rssi_FXD3+rssi_MVB3)/2);
  Serial.print("Sending Messages: ");
  Serial.println(messages);
  client.publish(outTopic, messages);
  delay(100);
  
  // Plot last received RSSI readings from fixed nodes
  sprintf(plot, "(%d,%d,%d)", rssi_MVB1, rssi_MVB2, rssi_MVB3);
  Serial.print("Received RSSI: ");
  Serial.println(plot); 
}
