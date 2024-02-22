#ifdef ESP8266
 #include <ESP8266WiFi.h>
 #else
 #include <WiFi.h>
#endif
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <iostream>
#include <string.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <sstream>
#define atoa(x) #x
#include <stdio.h>
#include <stdlib.h>


// Deklarasi PIN Controller
#define PIN_AIR_MASUK_TANDON D5
#define PIN_AIR_KELUAR_TANAMAN D6
#define PIN_PUPUK D3
#define PIN_4 D4
// topic
bool t_amt = false;
bool t_akt = false;
bool t_p = false;
bool t_ppm = false;
// value
bool v_amt = false;
bool v_akt = false;
bool v_p = false;

// time
bool t_auto = false;


// BUFFER SIZE FOR MQTT
#define MSG_BUFFER_SIZE (50)

/**WiFI Connections Details**/
const char* ssid = "wildan"; // set your ssid
const char* password = "ug111111"; // set your password

/**MQTT Broker Connection Details**/
const char* mqtt_server = "b579eab42dbe4d60a49f09a4f513b74d.s1.eu.hivemq.cloud";
const char* mqtt_username = "bisaa";
const char* mqtt_password = "Yabisadong11";
const int mqtt_port = 8883;
const char* mqtt_publish_ultrasonik = "ultrasonik/sensor/water-level";
const char* mqtt_publish_tds = "tds/sensor/ppm-value";
const char* mqtt_subscribe_air_masuk_tandon = "controller/air-masuk-tandon";
const char* mqtt_subscribe_air_keluar_tanaman = "controller/air-keluar-tanaman";
const char* mqtt_subscribe_pupuk = "controller/pupuk";
const char* mqtt_subscribe_set_ppm = "tds/sensor/ppm-value/set-ppm"; 
const char* mqtt_subscribe_current_ppm = "tds/sensor/ppm-value";
const char* test_time = "1970-01-20T14:24:22";
const char* mqtt_publish_state_pupuk = "controller/state-pupuk";
const char* mqtt_publish_state_water = "controller/state-water";

// Cert Root Server
const char* ca_cert= "-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";


//Secure WiFi Connectivity Initialisation
WiFiClientSecure espClient;

//MQTT Client Initialisation Using WiFi Connection
PubSubClient client(espClient);

/**Store Buffer Message**/
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];

const long utcOffsetInSeconds = 25200;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

unsigned long setPpm = 0;

unsigned long currentPpm = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // connect to wifi
  WiFi.begin(ssid, password);

  Serial.print("\nConnecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  #ifdef ESP8266
    espClient.setInsecure();
  #else
    espClient.setCACert(root_ca);      // enable this line and the the "certificate" code for secure connection
  #endif

  // connect to MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // pin mode controller
  pinMode(PIN_AIR_MASUK_TANDON, OUTPUT);
  pinMode(PIN_AIR_KELUAR_TANAMAN, OUTPUT);
  pinMode(PIN_PUPUK, OUTPUT);

  // setup HIGH first every PIN MODE
  digitalWrite(PIN_AIR_MASUK_TANDON, HIGH);
  digitalWrite(PIN_AIR_KELUAR_TANAMAN, HIGH);
  digitalWrite(PIN_PUPUK, HIGH);

  timeClient.begin();
}

void loop() {

  // subscribe set ppm
 
  if(!client.connected()) recconect();
  client.loop();
  
  if(t_amt){
    if(v_amt) airMasukTandonOn();
    else airMasukTandonOff();
  }

  if(t_akt){
    if(v_akt) airKeluarTanamanOn();
    else airKeluarTanamanOff();
  }

  if(t_p){
    if(v_p) pupukOn();
    else pupukOff();
  }

  if(setPpm != 0 && currentPpm !=0){
    // if currentPPm less than setPpm add pupuk otherwise add water
    int start = setPpm - 50;
    int end = setPpm + 50;
    if(currentPpm >= start && currentPpm <= end){
      publishMessage(mqtt_publish_state_pupuk, "0", true);
      publishMessage(mqtt_publish_state_water, "0", true);
      airMasukTandonOff();
      pupukOff();
    }else if(currentPpm < start){
      publishMessage(mqtt_publish_state_pupuk, "1", true);
      publishMessage(mqtt_publish_state_water, "0", true);
      // pupuk on, water off
      airMasukTandonOff();
      pupukOn();
    }else{
      publishMessage(mqtt_publish_state_pupuk, "0", true);
      publishMessage(mqtt_publish_state_water, "1", true);
      // water on, pupuk off
      airMasukTandonOn();
      pupukOff();
    }
  }
  
  Serial.println(setPpm);
  Serial.println(currentPpm);
  
  delay(1000);  // Memberi jeda antar loop
}

// method controller to handle tandon air ON
void airMasukTandonOn(){
  digitalWrite(PIN_AIR_MASUK_TANDON, LOW);
}

// method controller to handle tandon air OFF
void airMasukTandonOff(){
  digitalWrite(PIN_AIR_MASUK_TANDON, HIGH);
}

// method controller to handle air keluar ON
void airKeluarTanamanOn(){
  digitalWrite(PIN_AIR_KELUAR_TANAMAN, LOW);
}

// method controller to handle air keluar OFF
void airKeluarTanamanOff(){
  digitalWrite(PIN_AIR_KELUAR_TANAMAN, HIGH);
}

// method controller to handle pupuk ON
void pupukOn(){
  digitalWrite(PIN_PUPUK, LOW);
}

// method controller to handle pupuk OFF
void pupukOff(){
  digitalWrite(PIN_PUPUK, HIGH);
}

// to recconect MQTT
void recconect(){
  // Loop unit we're reconnected
  while(!client.connected()){
    Serial.print("Attempting MQTT Connection:");
    String clientId = "ESP32";
    //clientId+= String(random(0xffff), HEX);

   // attempt to connect
   if(client.connect(clientId.c_str(), mqtt_username, mqtt_password)){
    Serial.println(" Connected");
    client.subscribe(mqtt_subscribe_air_masuk_tandon);
    client.subscribe(mqtt_subscribe_air_keluar_tanaman);
    client.subscribe(mqtt_subscribe_pupuk);
    client.subscribe(mqtt_subscribe_set_ppm);
    client.subscribe(mqtt_subscribe_current_ppm);
   }else{
    Serial.print("failed, rc =");
    Serial.print(client.state());
    Serial.println("try again in 5 second");
    delay(5000);
   }
  }
}


// To get callback MQTT
void callback(char* topic, uint8_t* payload, unsigned int length){
  String incomingMsg = "";
  for(int i = 0; i < length;i++){
    incomingMsg += (char)payload[i];
  }
  Serial.print("nilai ");
  Serial.println(incomingMsg);
  Serial.println(topic);

  // check topic
  const char* im = incomingMsg.c_str();
  
  // air masuk tandon
  if(strcmp(mqtt_subscribe_air_masuk_tandon, topic) == 0){
    t_amt = true;
    if(strcmp(im, "1") == 0){
      v_amt = true;
    }else{
      v_amt = false;
    }
  }

  // air keluar tanaman
  if(strcmp(mqtt_subscribe_air_keluar_tanaman, topic) == 0){
    t_akt = true;
    if(strcmp(im, "1") == 0){
      v_akt = true;
    }else{
      v_akt = false;
    }
  }

  // pupuk
  if(strcmp(mqtt_subscribe_pupuk, topic) == 0){
    t_p = true;
    if(strcmp(im, "1") == 0){
      v_p = true;
    }else{
      v_p = false;
    }
  }

  // set ppm
  if(strcmp(mqtt_subscribe_set_ppm, topic) == 0){
    setPpm = atoi(im);
  }

  // currentPPm
  if(strcmp(mqtt_subscribe_current_ppm, topic) == 0){
    // Parse JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, im);
   // Check for parsing errors
  if (error) {
    Serial.print("Parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Get sensor_tds value
  const char* valuePpm = doc["sensor_tds"];

  // Print sensor_tds value
  Serial.print("sensor_tds value: ");
  Serial.println(valuePpm); 

  currentPpm = atoi(valuePpm);  
  }
  
}

// to subscribe topic
void subscribeMessage(const char* topic, String payload, boolean retained){
  client.subscribe(topic);
}

void currentTime() {
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.print(":");
  Serial.println(timeClient.getEpochTime() % 1000); 
  Serial.print(":");
  Serial.println(timeClient.getEpochTime()); 
  Serial.println(timeStringToMillis(test_time));

  // start
  if(timeClient.getHours() >= 15){
    if(timeClient.getMinutes() >= 15){
      t_auto = true;
    } 
  }

    // start
  if(timeClient.getHours() >= 15){
    if(timeClient.getMinutes() >= 15){
      t_auto = true;
    } 
    
  }
  

  char destination[] = "1970-01-20T";
  char test [] = "1970-01-20T15:28:2";
  sprintf(destination, "1970-01-20T%d:%d:00", timeClient.getHours(), timeClient.getMinutes());

  Serial.println(destination);
  Serial.println(timeStringToMillis(test));
  // if else

  delay(1000);
}

unsigned long timeStringToMillis(const char* timeString) {
    int hours, minutes, seconds;
    sscanf(timeString, "%d:%d:%d", &hours, &minutes, &seconds);

    // Calculate total milliseconds since midnight
    unsigned long totalMillis = hours * 3600000UL + minutes * 60000UL + seconds * 1000UL;

    return totalMillis;
}

void publishMessage(const char* topic, String payload, boolean retained){
  if(client.publish(topic, payload.c_str(), true)){
    Serial.println(payload);
  }
}
