#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

#include <Arduino.h>
#include <WiFiMulti.h>
  #include <WiFi.h>

#include <Firebase_ESP_Client.h>

DHT dht(DHTPIN, DHTTYPE);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials


// Insert Firebase project API Key
#define API_KEY "AIzaSyDJlX3o3c7UBa4qaQpYjyIzKbX6VXwXMm4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://dht-inventory-default-rtdb.firebaseio.com/" 
WiFiMulti wifiMulti;
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;
String firestatus="";
int fsensor=4;
 
void setup(){
  wifiMulti.addAP("V2029", "87654327");
wifiMulti.addAP("UTeM-Net","1UTeM@PPPK");
 
  pinMode(fsensor, INPUT);
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
 delay(1000);
  float h = dht.readHumidity();

  float t = dht.readTemperature();

 
  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/kelembapan",h)){
//      Serial.println("PASSED");
       Serial.print("Kelembapan: ");
       Serial.println(h);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/suhu", t)){
//      Serial.println("PASSED");
       Serial.print("Suhu: ");
       Serial.println(t);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
     int fire = digitalRead(fsensor);// read Flame sensor
 
 if( fire == LOW)
  {
    firestatus="smoke_Detected";
    Firebase.RTDB.setString(&fbdo, "DHT/Status", firestatus);
    Serial.println("Fire Detected");
  }
  if (fire == HIGH)
  {
    firestatus="No_smoke";
    Firebase.RTDB.setString(&fbdo, "DHT/Status", firestatus);
 
    Serial.println("No Fire");
  }
  else {
  firestatus="No_smoke";
    Firebase.RTDB.setString(&fbdo, "DHT/Status", firestatus);
 
    Serial.println("No Fire");
  
  }
  Serial.println("______________________________");
}
