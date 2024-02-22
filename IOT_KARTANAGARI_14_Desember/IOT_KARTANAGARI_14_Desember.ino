/**Import Library**/
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <NTPClient.h>  
#include "time.h"
#include <WiFiUdp.h>
using namespace std;

/**deklarasi variable ULTRASONIK**/
#define TRIG_PIN 12
#define ECHO_PIN 13
#define BATAS_ATAS 50
#define BATAS_BAWAH 10
float measure, distance_cm;

/**deklarasi variable TDS SENSOR**/
#define TdsSensorPin 33
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point
int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation

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

/**Define NTP Client (TO GET Current Time in Arduino)**/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Variables to save date and time
String formattedTime;
String dayStamp;
String timeStamp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // connect to WIFI
  WiFi.begin(ssid, password);
  
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");

  get_network_info();

  // connect to MQTT
  espClient.setCACert(ca_cert);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Initialize and synchronize time
  timeClient.begin();
  timeClient.update();
  timeClient.setTimeOffset(25200);
  
  // pin mode ultrasonik
  pinMode(TRIG_PIN, OUTPUT);   // set ESP32 pin to output mode
  pinMode(ECHO_PIN, INPUT);    // set ESP32 pin to input mode

  // pin mode Tds
  pinMode(TdsSensorPin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(!client.connected()) recconect();
  client.loop();
  timeClient.update();
    
  // call Ultrasonik function
  ultrasonik();

  // call TDS Sensor function
  tds1();

  delay(5000);
}

// Method to read Ultrasonik Sensor
void ultrasonik(){
  // Declare Dynamic JSON
  DynamicJsonDocument doc(1024);

  // Declare mqtt_message
  char mqtt_message[128];
  
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  measure = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * measure;

  // bakal cek ketika > 10 kurang dari threshold
  if(distance_cm > BATAS_BAWAH && distance_cm < BATAS_ATAS){
    doc["sensor_ultrasonik"] = "1";
    serializeJson(doc, mqtt_message);
    publishMessage(mqtt_publish_ultrasonik, mqtt_message, true);
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    Serial.print("value: ");
    Serial.println(true);
  }else{
    doc["sensor_ultrasonik"] = "0";
    serializeJson(doc, mqtt_message);
    publishMessage(mqtt_publish_ultrasonik, mqtt_message, true);
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    Serial.print("value: ");
    Serial.println(false);
  }
}

// To Get Network Information
void get_network_info(){
    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("[*] Network information for ");
        Serial.println(ssid);

        Serial.println("[+] BSSID : " + WiFi.BSSIDstr());
        Serial.print("[+] Gateway IP : ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("[+] Subnet Mask : ");
        Serial.println(WiFi.subnetMask());
        Serial.println((String)"[+] RSSI : " + WiFi.RSSI() + " dB");
        Serial.print("[+] ESP32 IP : ");
        Serial.println(WiFi.localIP());
    }
}

// To get callback MQTT
void callback(char* topic, byte* payload, unsigned int length){
  String incomingMsg = "";
  for(int i = 0; i < length;i++){
    incomingMsg += (char)payload[i];
  }
  Serial.println(incomingMsg);
}

// Method to reconnect MQTT Server
void recconect(){
  // Loop unit we're reconnected
  while(!client.connected()){
    Serial.print("Attempting MQTT Connection:");
    String clientId = "ESP32";
    //clientId+= String(random(0xffff), HEX);

   // attempt to connect
   if(client.connect(clientId.c_str(), mqtt_username, mqtt_password)){
    Serial.println(" Connected");
   }else{
    Serial.print("failed, rc =");
    Serial.print(client.state());
    Serial.println("try again in 5 second");
    delay(5000);
   }
  }
}


// Method to publish message in mqtt
void publishMessage(const char* topic, String payload, boolean retained){
  if(client.publish(topic, payload.c_str(), true)){
    Serial.println(payload);
  }
}

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void tds()
{
   // Declare Dynamic JSON
   DynamicJsonDocument doc(1024);

   // Declare mqtt_message
   char mqtt_message[128];
   Serial.println(analogRead(TdsSensorPin));

   
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      Serial.print("voltage:");
      Serial.print(averageVoltage,2);
      Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
      int convert = (int) tdsValue;
      string float_to_string = to_string(convert);
      doc["sensor_tds"] = float_to_string;
      serializeJson(doc, mqtt_message);
      publishMessage(mqtt_publish_tds, mqtt_message, true);
   }
}

void tds1()
{
   // Declare Dynamic JSON
   DynamicJsonDocument doc(1024);

   // Declare mqtt_message
   char mqtt_message[128];
   
   float tdsValue = analogRead(TdsSensorPin);
   int convert = (int) tdsValue;
   string float_to_string = to_string(convert);
   doc["sensor_tds"] = float_to_string;
   doc["time_tds"] = timeClient.getFormattedTime();
   serializeJson(doc, mqtt_message);
   publishMessage(mqtt_publish_tds, mqtt_message, true);
}
