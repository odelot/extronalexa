#include <Arduino.h>
#include <Stream.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include "sha256.h"
#include "Utils.h"
#include <Hash.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"
extern "C" {
  #include "user_interface.h"
}

#define SOFTSERIAL_RX D1
#define SOFTSERIAL_TX D2
#include <SoftwareSerial.h>

SoftwareSerial mySerial(SOFTSERIAL_RX,SOFTSERIAL_TX);
/**********************************************************/

//AWS IOT config, change these:
char wifi_ssid[]       = "your wifi ssid";
char wifi_password[]   = "your wifi password";
char aws_endpoint[]    = "yourEndpoint.iot.us-west-2.amazonaws.com";
char aws_key[]         = "your key";
char aws_secret[]      = "your secret";
char aws_region[]      = "us-west-2";

int port = 443;

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

int latchPin = D7;
int clockPin = D6;
int dataPin = D5;

ESP8266WiFiMulti WiFiMulti;

AWSWebSocketClient awsWSclient(1000);

PubSubClient client(awsWSclient);

//AWS root certificate - expires 2037
const char ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEkjCCA3qgAwIBAgITBn+USionzfP6wq4rAfkI7rnExjANBgkqhkiG9w0BAQsF
ADCBmDELMAkGA1UEBhMCVVMxEDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNj
b3R0c2RhbGUxJTAjBgNVBAoTHFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4x
OzA5BgNVBAMTMlN0YXJmaWVsZCBTZXJ2aWNlcyBSb290IENlcnRpZmljYXRlIEF1
dGhvcml0eSAtIEcyMB4XDTE1MDUyNTEyMDAwMFoXDTM3MTIzMTAxMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaOCATEwggEtMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/
BAQDAgGGMB0GA1UdDgQWBBSEGMyFNOy8DJSULghZnMeyEE4KCDAfBgNVHSMEGDAW
gBScXwDfqgHXMCs4iKK4bUqc8hGRgzB4BggrBgEFBQcBAQRsMGowLgYIKwYBBQUH
MAGGImh0dHA6Ly9vY3NwLnJvb3RnMi5hbWF6b250cnVzdC5jb20wOAYIKwYBBQUH
MAKGLGh0dHA6Ly9jcnQucm9vdGcyLmFtYXpvbnRydXN0LmNvbS9yb290ZzIuY2Vy
MD0GA1UdHwQ2MDQwMqAwoC6GLGh0dHA6Ly9jcmwucm9vdGcyLmFtYXpvbnRydXN0
LmNvbS9yb290ZzIuY3JsMBEGA1UdIAQKMAgwBgYEVR0gADANBgkqhkiG9w0BAQsF
AAOCAQEAYjdCXLwQtT6LLOkMm2xF4gcAevnFWAu5CIw+7bMlPLVvUOTNNWqnkzSW
MiGpSESrnO09tKpzbeR/FoCJbM8oAxiDR3mjEH4wW6w7sGDgd9QIpuEdfF7Au/ma
eyKdpwAJfqxGF4PcnCZXmTA5YpaP7dreqsXMGz7KQ2hsVxa81Q4gLv7/wmpdLqBK
bRRYh5TmOTFffHPLkIhqhBGWJ6bt2YFGpn6jcgAKUj6DiAdjd4lpFw85hdKrCEVN
0FE6/V1dN2RMfjCyVSRCnTawXZwXgWHxyvkQAiSr6w10kY17RSlQOYiypok1JR4U
akcjMS9cmvqtmg5iUaQqqcT5NJ0hGA==
-----END CERTIFICATE-----
)EOF";



//# of connections
long connection = 0;

char myTopic[]      = "fabio/casa/gameroom";

StaticJsonDocument<200> doc;

//subscribe to a mqtt topic
void subscribe () {
    client.setCallback(callback);
    client.subscribe(myTopic);
    client.subscribe("fabio/casa/gameroom/#");
    client.subscribe("fabio/casa/#");
   //subscript to a topic
    Serial.println("MQTT subscribed");
}


//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

//count messages arrived
int arrivedcount = 0;


void switchCommand() {
  int channel = atoi ((const char*)doc["in"]);
  char tmp[6];
  Serial.println(channel);
  if (channel != 0) {
    tmp[0]=0x1b;
    strcpy(tmp+1,(const char*)doc["in"]);
    tmp[2]='*';
    tmp[3]='!';
    tmp[4]=0x0d;
    tmp[5]=0x00;
    
  } else {
    tmp[0]=0x1b;
    tmp[1]='1';
    tmp[2]='*';
    tmp[3]='B';
    tmp[4]=0x0d;
    tmp[5]=0x00;
  }
  
  Serial.print(tmp);
  lightCircle(channel);
  mySerial.print(tmp);
}

//callback to handle mqtt messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  DeserializationError error = deserializeJson(doc, payload);
  if (doc["command"]=="switch") {
    switchCommand();
  }
}

//connects to websocket layer and mqtt layer
bool connect () {
    if (client.connected()) {    
        client.disconnect ();
    }  
    //delay is not necessary... it just help us to get a "trustful" heap space value
    delay (1000);
    Serial.print (millis ());
    Serial.print (" - conn: ");
    Serial.print (++connection);
    Serial.print (" - (");
    Serial.print (ESP.getFreeHeap ());
    Serial.println (")");


    //creating random client id
    char* clientID = generateClientID ();
    
    client.setServer(aws_endpoint, port);
    if (client.connect(clientID)) {
      Serial.println("connected");     
      return true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      return false;
    }
    
}



void lightCircle (int channel) {
  byte number = 0;
  
  if (channel > 0) {
    number = 0x01;
    number = number << (channel-1);
  }
  Serial.println(number);
   //count up routine
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, number);
  //return the latch pin high to signal chip that it
  //no longer needs to listen for information
  digitalWrite(latchPin, HIGH);
}


void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  wifi_set_sleep_type(NONE_SLEEP_T);
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  lightCircle (0);

  
  //fill with ssid and wifi password
  WiFiMulti.addAP(wifi_ssid, wifi_password);
  Serial.println ("connecting to wifi");
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
      Serial.print (".");
  }
  Serial.println ("\nconnected");

  setClock(); // Required for X.509 certificate  validation
  
  //fill AWS parameters    
  awsWSclient.setAWSRegion(aws_region);
  awsWSclient.setAWSDomain(aws_endpoint);
  awsWSclient.setAWSKeyID(aws_key);
  awsWSclient.setAWSSecretKey(aws_secret);
  awsWSclient.setUseSSL(true);
  awsWSclient.setCA(ca);
  //as we had to configurate ntp time to validate the certificate, we can use it to validate aws connection as well
  awsWSclient.setUseAmazonTimestamp(false);

  if (connect()){
    sendEchoMessage();
    subscribe();    
  }
  

  
}

void loop() {
  if (awsWSclient.connected ()) {    
      client.loop ();
  } else {
    //handle reconnection
    if (connect ()){
      subscribe ();      
    }
  }      
}