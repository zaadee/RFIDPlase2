/**
    Interfacing Node MCU with Arduino
    |------------------------------------|
    |            Node MCU    Arduino     |
    |            ESP8266     Uno 3       |
    |Signal      Pin         Pin         |
    |------------------------------------|
    |VIN         VIN         VIN         |
    |GND         GND         GND         |
    |RX          D3          D3          |
    |TX          D2          D2          |
    -------------------------------------


*/
/*
   Result code to comunication NodeMCU with Arduino.
*/
#define RES_SUCCESS 1           //Code: 1 Send data to server success Car in.
#define RES_SUCCESS 2           //Code: 2 Send data to server success Car out.
#define RES_NO_INTERNET 3       //Code: 3 NodeMCU can't connect WiFi.
#define RES_ERROR 4             //Code: 4 Connection error.

/**
   LED
*/
#define LED_GREEN D5
#define LED_RED D6

//WiFi ---------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "Room309";  //WiFi name
const char* password = "password";    //Passord
const char* serverApi = "http://192.168.137.248"; //Server ip address
const int serverPort = 80;            //port
HTTPClient http;

//Serial ---------------------------------
#include <SoftwareSerial.h>
SoftwareSerial NodeSerial(D2, D3); // RX | TX

//Json -----------------------------------
#include <ArduinoJson.h>

//Valiable --------------------------------
//String data;

//Functions!!!-----------------------------
void connectWiFi() {
  Serial.print(F("Connecting to: ")); Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    digitalWrite(LED_RED, !digitalRead(LED_RED));
  }
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: ")); Serial.println(WiFi.localIP());
}
/**
   Listener Arduino un send data drom serial port.
*/
void serialListener() {
  String data = ""; //clear data
  while (NodeSerial.available() > 0) {
    digitalWrite(LED_RED, !digitalRead(LED_BUILTIN));
    char character = NodeSerial.read();
    Serial.print("Character: "); Serial.println(String(character));
    data.concat(character);
  }
  
  delay(10);
  if (data != "") {
    Serial.println("-----------------------------");
    digitalWrite(LED_RED, LOW);
    Serial.println(("Data From Arduino: " + data));

    String carData;
    String driverData;
    int commaIndex = data.indexOf(',');
    carData = data.substring(0, commaIndex);
    driverData = data.substring((commaIndex + 1));
    Serial.println("Car: " + carData + ", Driver: " + driverData);

    if ((carData != "") && (driverData != "") && (carData.charAt(0) == 'C') && (carData.length() == 6) &&
        (driverData.charAt(0) == 'D') && (driverData.length() == 6)) {
      postData(carData, driverData);
    } else {
      digitalWrite(LED_RED, HIGH);
      onResponse(400);
      delay(1000);
      digitalWrite(LED_RED, LOW);
    }
  }
}
/**
   Send result code to arduino uno
*/
void onResponse(int result) {
  NodeSerial.print(result);
}
/**
   Post data to server
*/
void postData(String carData, String driver) {
  digitalWrite(LED_GREEN, LOW);
  Serial.println("Post data");
  http.setTimeout(2000);
  http.begin("http://192.168.137.248/rfid/api/?action=post_in_out");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("car=" + carData + "&driver=" + driver);
  digitalWrite(LED_GREEN, HIGH);
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    //Serial.println(payload);             //Print the response payload
    createResponse(payload);
  } else {
    Serial.printf("[HTTP   ] POST... failed with code: %d, error: %s\n", httpCode, http.errorToString(httpCode).c_str());
    digitalWrite(LED_RED, HIGH);
    delay(1000);
    digitalWrite(LED_RED, LOW);
  }
  http.end();
}
/**
   Create response
*/
void createResponse(String payload) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root  = jsonBuffer.parseObject(payload);
  String Status = root["status"];
  int results = root["results"];
  String message = root["message"];

  Serial.println("Status: " + Status);
  Serial.println("Result: " + String(results));
  Serial.println("Message: " + message);

  onResponse(results);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
}
void getExample() {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin("http://jsonplaceholder.typicode.com/users/1");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();   //Close connection
  }
  delay(30000);    //Send a request every 30 seconds
}
void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  delay(10);
  NodeSerial.begin(115200);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  connectWiFi();
  Serial.println(F("Start.."));
}
void loop() {
  //Blink LDD Example
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //delay(1000);

  //test post data
  //postData("C00478", "D00003");

  serialListener();
}
