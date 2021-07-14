#include <WiFi.h>
#include <PubSubClient.h>

// AP configuration ID // UNCOMMENT FOR X=1,2,3
//const char *ssid_own = "REF1_node"; 
//const char *ssid_own = "REF2_node";
const char *ssid_own = "REF3_node";
const char* pwd_own  = "00000000";

// Local NET configuration ID
const char* ssid_net = "your_network_ssid";
const char* pwd_net  = "your_network_pwd";

// MQTT configuration
const char* brokerUser = "mqtt";
const char* brokerPass = "password";
const char* broker = "192.168.0.6";
unsigned mqtt_port = 1883;

// out topics to publish and in topics to subscribe // UNCOMMENT FOR X=1,2,3
//const char* outTopic ="/RSSI_1";
//const char* outTopic ="/RSSI_2";
const char* outTopic ="/RSSI_3";

// AP movable nodes variables 
const char *ssid_AP0 = "REF0_node";

// Initialize Wi-Fi AP server and MQTT client
WiFiServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

long rssi_MVB1; // RSSI readings in each FXD_ND_X node


void scan_mvb_rssi(){ // scan wi-fi networks for rssi of movable nodes
  char plot[64];
	int nNet = WiFi.scanNetworks(); // number of wifi networks
    if (nNet == 0) {
        Serial.println("NULL");
    } else {
        for (int i = 0; i < nNet; ++i) {
            // Print SSID and RSSI for specific fixed AP nodes 
            boolean flag_AP0 = WiFi.SSID(i).equals(ssid_AP0);

            // store fixed nodes rssi
			      if(flag_AP0){rssi_MVB1 = (long) WiFi.RSSI(i);}
            }
    }
    // Plot last RSSI readings in movable node
    sprintf(plot, "(%d)", rssi_MVB1);
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

void reconnect(){ // reconnect to broker (there is no topics to subscribe to)
  while(!client.connected()){
    Serial.print("Connecting to ");
    Serial.println(broker);
    //if(client.connect("REF1", brokerUser, brokerPass)){ // UNCOMMENT FOR X=1,2,3
	  //if(client.connect("REF2", brokerUser, brokerPass)){ // UNCOMMENT FOR X=1,2,3
	  if(client.connect("REF3", brokerUser, brokerPass)){ // UNCOMMENT FOR X=1,2,3
      Serial.print("Connected to ");
      Serial.println(broker);
    } else { // retry connecting if falied
      Serial.println("\n Trying to reconnect");
      delay(100);
    }
  }
}

 void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_AP_STA);
  init_AP();
  init_STA();
  
  client.setServer(broker, mqtt_port);
  // there is no need to set callback since it is not subscribed to any topic
  
  delay(10);
}

void loop() {
  char plot[64];
  char messages[64];
  
  scan_mvb_rssi();
  if (!client.connected()){ // check connection to broker
      reconnect();
    }
  client.loop();
  
  // Publish message to outTopic
  sprintf(messages, "%d", (long)rssi_MVB1);
  Serial.print("Sending Messages: ");
  Serial.println(messages);
  client.publish(outTopic, messages);
  delay(100);
}
