#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include "GravityTDS.h"

#define TdsSensorPin A0
GravityTDS gravityTds;
float temperature = 25,tdsValue = 0;

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Treasure"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBfKGRZmmC3ecTQ0INMhsTw0Ds_c1e_9UQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smart-hydro-app-2f0c8-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


bool signupOK = false;
int kontrolNutrisi;

// Sensor LDR (Cahaya)
int cahaya=D1;

// Relay 4 chanel
int r1 = D4;
int r2 = D6;
int r3 = D7;

// Sensor Suhu Air
#define ONE_WIRE_BUS D5 // definisikan pin yang terhubung ke sensor DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Membuat instance dari kelas OneWire
DallasTemperature sensors(&oneWire); // Membuat instance dari kelas DallasTemperature


void setup(){
  pinMode(cahaya,INPUT); // Mengubah mode pin cahaya menjadi input
  pinMode (r1, OUTPUT);
  pinMode (r2, OUTPUT);
  pinMode (r3, OUTPUT);

  sensors.begin();

    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
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

  int data = analogRead(cahaya); // inisialisasi data untuk membaca data anaog dari sensor ldr
  // Meminta pembacaan suhu dari sensor DS18B20
  sensors.requestTemperatures();
  //Membaca suhu air dalam Celsius dari sensor pertama(indek 0)
  float temperatureC = sensors.getTempCByIndex(0);  
  
  String keadaan = "";

  //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
  
  if(data > 500){
    digitalWrite(r3, HIGH);
    keadaan = "Gelap";
  } else {
  digitalWrite(r3, LOW);
  keadaan = "Terang";
  }

    
  if(tdsValue < 0.8 * kontrolNutrisi){
    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
  } else {
    digitalWrite(r1,LOW);
    digitalWrite(r2,LOW);
  }
  
  if (Firebase.ready() && signupOK ) {

    if (Firebase.RTDB.setFloat(&fbdo, "Nutrisi/sensor", tdsValue)){
       Serial.print("Ppm: ");
       Serial.println(tdsValue);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "Cahaya/Keadaan", keadaan)){
       Serial.print("Keadaan: ");
       Serial.println(keadaan);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "Cahaya/Intensitas", data)){
       Serial.print("Intensitas: ");
       Serial.println(data);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
    if (Firebase.RTDB.setFloat(&fbdo, "suhu_air/Celcius", temperatureC)){
       Serial.print("Suhu Air: ");
       Serial.println(temperatureC);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    Serial.println("----- Fetch Data Sensor -----");
    
    if (Firebase.RTDB.getInt(&fbdo, "Nutrisi/kontrol")) {
       kontrolNutrisi = fbdo.intData();
       Serial.print("Get Data Ppm Sensor: ");
       Serial.println(kontrolNutrisi);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
  }
  Serial.println("__________");
}
