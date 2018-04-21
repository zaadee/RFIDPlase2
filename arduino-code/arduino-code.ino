/*
   Interfacing  Arduino with RFID-RC522
   |------------------------------------|
   |            Arduino     MFRC522     |
   |            Uno 3       Reader/PCD  |
   |Signal      Pin         Pin         |
   |------------------------------------|
   |RST/Reset   D9          RST         |
   |SPI SS      D10         SDA(SS)     |
   |SPI MOSI    D11         MOSI        |
   |SPI MISO    D12         MISO        |
   |SPI SCK     D13         SCK         |
   -------------------------------------

   Interfacing  Arduino with Node MCU
   |------------------------------------|
   |            Arduino     Node MCU    |
   |            Uno 3       ESP8266     |
   |Signal      Pin         Pin         |
   |------------------------------------|
   |VIN         VIN         VIN         |
   |GND         GND         GND         |
   |RX          D3          D3          |
   |TX          D2          D2          |
   -------------------------------------

   Interfacing  Arduino with LM1602
   |------------------------------------|
   |            Arduino     LM1602      |
   |            Uno 3       I2Z         |
   |Signal      Pin         Pin         |
   |------------------------------------|
   |GND         GND         GND         |
   |VCC         5V          VCC         |
   |SDA         A4          SDA         |
   |SCL         A5          SCL         |
   -------------------------------------
*/
/*
   Result code to comunication NodeMCU with Arduino.
*/
#define RES_SUCCESS 1           //Code: 1 Send data to server success.
#define RES_NO_INTERNET 2       //Code: 2 NodeMCU can't connect WiFi

/**
   Code state on working
*/
#define STATE_NORMAL 0
#define STATE_LOGIN 1
#define STATE_CRAETE_CARD 2
#define STATE_WRITE_CARD 3
#define STATE_WAIT_CARD 4
#define STATE_WAIT_RESPONSE 5
#define STATE_CHECK_IN_OUT_WITH_KEYPAD 6

//LCD I2C ------------------------------
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//RFID-RC522 Shield --------------------
#include <SPI.h>
#include <MFRC522.h>

int BUZZER = 8;
constexpr uint8_t RST_PIN = 9;  // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;  // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte sector         = 1;
byte blockAddr      = 4;
byte trailerBlock   = 7;
byte dataBlock[16]  = {0};
byte buffer[18];
byte size = sizeof(buffer);

//Keypad --------------------------------
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[4] = { A3, A2, A1, A0 }; //(1-4 fron left on keypad)
byte colPins[4] = { 7, 6, 5, 4, }; //(5-8 fron left on keypad)
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
String input; //Value when keypad.

//Serial ---------------------------------
#include <SoftwareSerial.h>
SoftwareSerial ArduinoSerial(3, 2); // RX | TX

//Valiable --------------------------------
byte STATE;
int timer;
String carData;
String driverData;
//-----------------------------------------

//Functions!!!-----------------------------
/**
  Lcd print message first line only.
*/
void lcdPrint(String message) {
  lcd.clear();
  for (byte i = 0; i < message.length(); i++) {
    if (i == 16) {
      lcd.setCursor(0, 1);
    }
    lcd.print(message[i]);
  }
  message = "";
}

/**
   lcd print message tow line.
*/
void lcdPrint(String messageLine1, String messageLine2) {
  lcd.clear();
  for (byte i = 0; i < messageLine1.length(); i++) {
    lcd.print(messageLine1[i]);
  }
  lcd.setCursor(0, 1);
  for (byte i = 0; i < messageLine2.length(); i++) {
    lcd.print(messageLine2[i]);
  }
  messageLine1 = "";
  messageLine2 = "";
}
void printSerialEnd() {
  Serial.println(F("--------------------------------------"));
}
/**
   Set state of working.
*/
void setState(byte state) {
  STATE = state;
  printSerialEnd();
  if (STATE == STATE_NORMAL) {
    Serial.println(F("State 0: Listener Keypad and Rfid"));
    lcdPrint("Wellcome!", "Tracking card...");
  } else if (STATE == STATE_LOGIN) {
    Serial.println(F("State 1: Login to system"));
    Serial.println(F("Enter password. Press # to next"));
    lcdPrint("Enter password");
  } else if (STATE == STATE_CRAETE_CARD) {
    Serial.println(F("State 2: Fill in data for write Rfid card"));
    Serial.println(F("Enter card data. Press # to next"));
    lcdPrint("Enter card data");
  } else if (STATE == STATE_WRITE_CARD) {
    Serial.println(F("State 3: Write data to Rfid card"));
    Serial.println(F("Please Tracking Rfid card for write data..."));
    lcdPrint("Tracking card to", "write data...");
  } else if (STATE == STATE_WAIT_CARD) {
    Serial.println(F("State 4: Wait Remaining card"));
  } else if (STATE == STATE_WAIT_RESPONSE) {
    Serial.println(F("State 5: Send data to server..."));
    lcdPrint("Send data...");
  } else if (STATE == STATE_CHECK_IN_OUT_WITH_KEYPAD) {
    Serial.println(F("State 6: Check in out with keypad"));
    lcdPrint("Enter Car Id");
  } else if (STATE == 7) {

  }
}
void clearData() {
  carData = "";
  driverData = "";
  input = "";
  timer = 0;
}
bool isTimeout(int count) {
  timer += 1;
  //Serial.print(F("Timer: ")); Serial.print(timer); Serial.print(F(" in ")); Serial.println(count);
  if (timer > count) {
    clearData();
    return true;
  }
  return false;
}
/**
   Dump debug info about the card; PICC_HaltA() is automatically called
*/
void  dumpFRIDInfo() {
  rfid.PICC_DumpToSerial(&(rfid.uid));
}

/**
   Clear rfid when completed.
*/
void clearRfid() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

/**
   Read rfid card from secotr 1 (0-15 sector) block 4 (1-4)
*/
void readCardRfid() {
  Serial.println(F("Read RFID Card"));
  // Authenticate using key A
  //Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    lcdPrint("Read card failed");
    clearRfid();
    return;
  }

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector 1:"));
  rfid.PICC_DumpMifareClassicSectorToSerial(&(rfid.uid), &key, sector);
  Serial.println();

  // Read data from the block
  Serial.print(F("Reading data from block 4:"));
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    lcdPrint("Read card failed");
    clearRfid();
    return;
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();
  Serial.println();

  //Create data
  input = "";
  for (byte i = 0; i < 6; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.print(buffer[i], HEX);
    input += String::String(buffer[i], HEX);
  }
  input.toUpperCase();
  if (input.charAt(0) == 'C') {
    carData = input;
    Serial.print(F("Car data: ")); Serial.println(carData);
  } else if (input.charAt(0) == 'D') {
    driverData = input;
    Serial.print(F("Driver data: ")); Serial.println(driverData);
  } else {//If Rfid card data not math C(car) or D(Driver)
    Serial.println(F("Rfid card Incorrect!"));
    lcdPrint("Card Incorrect!");
    clearRfid();
    clearData();
    delay(2000);
    setState(STATE_NORMAL);
    return;
  }

  if (carData != "" && driverData != "") {
    //Send data to node mcu.
    lcdPrint(("Car: " + carData), ("Driver: " + driverData)); //C00123,D12345
    String postData = (carData + "," + driverData);
    ArduinoSerial.print(postData);
    setState(STATE_WAIT_RESPONSE);
  } else if (carData != "") {
    timer = 0; //Use timer wait remaining rfid card.
    setState(STATE_WAIT_CARD);
    Serial.print(F("Please tracking you card..."));
    lcdPrint("Car: " + carData, "Track you card...");
  } else if (driverData != "") {
    timer = 0; //Use timer wait remaining rfid card.
    setState(STATE_WAIT_CARD);//To next state waitRemainingcard.
    Serial.print(F("Please tracking car card..."));
    lcdPrint("Driver: " + driverData, "Track car card...");
  }
  clearRfid();
  //delay(2000);
}

//---------------------------------------------------------------------------------------------------------------
void writeCardRfid() {
  Serial.println(F("Write data to card"));
  // Authenticate using key B
  Serial.println(F("Authenticating again using key A..."));
  status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    lcdPrint("Write data failed!");
    clearRfid();
    return;
  }

  // Write data to the block
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(dataBlock, 16); Serial.println();
  status = (MFRC522::StatusCode) rfid.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    lcdPrint("Write data failed!");
    clearRfid();
    return;
  }
  Serial.println();
  Serial.println(F("Write data success"));
  lcdPrint("Write data", "Success!");
  clearRfid();
}
//---------------------------------------------------------------------------------------------------------------
bool rfidListener() {
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) {
    return false;
  }

  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return false;
  }
  String cardId;
  for (byte i = 0; i < 4; i++) {
    cardId += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX);
  }

  cardId.toUpperCase();
  Serial.println("Card Id: " + cardId);
  onBuzzer(100,1);
  return true;
}
//---------------------------------------------------------------------------------------------------------------
//Helper routine to dump a byte array as hex values to Serial.
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
void keypadListener() {
  char key = keypad.getKey();
  if (key ) {
    onBuzzer(100,1);
    if (key == '*') {
      clearData();
      setState(STATE_LOGIN);
    } else if (key == '#') {
      clearData();
      setState(STATE_CHECK_IN_OUT_WITH_KEYPAD);
    }
    else {
      Serial.println(F("Pressed * to Login"));
      lcdPrint("Press * to Login");
      delay(2000);
      setState(STATE_NORMAL);
    }
  }
}
//---------------------------------------------------------------------------------------------------------------
/**
  State 1:  Login to System.
          Enter password for login to write data (Car, Employee) in to RFID card.
          Password is 1234 (Can change it!)
*/
void loginToSystem() {
  String pass = "1234";
  char key = keypad.getKey();
  if (key ) {
    onBuzzer(100,1);
    timer = 0; //Whwn pressed keypad well be reset timer.
    if (key == '#') {
      if (input == pass) { //Login success. To next state fill in data for write rfid card.
        Serial.println(F("Login success!"));
        lcdPrint("Login success!");
        clearData();
        delay(1000);
        setState(STATE_CRAETE_CARD);
      } else { //password incorrect. Set state to default (0), Clear in put and delay 2 second.
        Serial.println(F("Password Incorrect!"));
        lcdPrint("Password", "Incorrect!");
        clearData();
        delay(2000);
        setState(STATE_NORMAL);
      }
    } else {
      input += String::String(key);
      Serial.print(F("Password: ")); Serial.println(input) ;
      lcdPrint("Enter password", input);

      if (input.length() > 16) {
        Serial.println(F("Password Incorrect!"));
        lcdPrint("Password", "Incorrect!");
        clearData();
        delay(2000);
        setState(STATE_NORMAL);
      }
    }
  }
  delay(30);
  if (isTimeout(1000)) { //Wait fild password 30 second
    Serial.println(F(""));
    Serial.println(F("Timeout wait login!"));
    lcdPrint("Timeout!", "Wait login");
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}
//---------------------------------------------------------------------------------------------------------------
/**
  State 2:  Fill in data for write Rfid Card.
            Enter data for write to rfid RFID card.
            Data type car enter       C00001, C00002.... (Can stor at 99,999 user)
            Data type employee enter  D00001, D00002.... (Can stor at 99,999 user)
*/
byte getBuyteUgly(char key) {
  if (key == '0') {
    return 0x00;
  } else if (key == '1') {
    return 0x01;
  } else if (key == '2') {
    return 0x02;
  } else if (key == '3') {
    return 0x03;
  } else if (key == '4') {
    return 0x04;
  } else if (key == '5') {
    return 0x05;
  } else if (key == '6') {
    return 0x06;
  } else if (key == '7') {
    return 0x07;
  } else if (key == '8') {
    return 0x08;
  } else if (key == '9') {
    return 0x09;
  } else if (key == 'A') {
    return 0x0A;
  } else if (key == 'B') {
    return 0x0B;
  } else if (key == 'C') {
    return 0x0C;
  } else if (key == 'D') {
    return 0x0D;
  }
  return 0x00;
}
void fillInDataForWriteRfidCard() {
  char key = keypad.getKey();
  if (key ) {
    onBuzzer(100,1);
    timer = 0; //Whwn pressed keypad well be reset timer.
    if (key == '#') {
      if (input.length() > 0) { //If have data. to next state Write data to Rfid card.
        Serial.println(F("Data in array: "));
        dump_byte_array(dataBlock, 16); Serial.println();
        setState(STATE_WRITE_CARD);
      } else { //not input data return to new state 2.
        Serial.println(F("Please Enter Data!."));
        lcdPrint("Please Enter ", "Data!");
        clearData();
        delay(2000);
        setState(STATE_CRAETE_CARD);
      }
    } else {
      input += String::String(key);
      Serial.print(F("Data: ")); Serial.println(input);
      lcdPrint("Enter card data", input);
      if ((input.length() - 1) <= 16) {
        dataBlock[(input.length() - 1)] = getBuyteUgly(key);
      } else {
        Serial.print(F("Input not format"));
        lcdPrint("Input not format");
        clearData();
        delay(2000);
        setState(STATE_CRAETE_CARD);
      }
    }
  }
  delay(30);
  if (isTimeout(1000)) { //Wait fild password 30 second
    Serial.println(F(""));
    Serial.println(F("Timeout wait fill data!"));
    lcdPrint("Timeout!", "Wait data");
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}
/**
  State 3:  Write data to Rfid Card.
            Tracking Rfid card for write (Variable: input)
*/
void writeDatatoRfidCard() {
  if (rfidListener()) {
    lcdPrint("Write data...");
    writeCardRfid();
    clearData();
    delay(2000);
    setState(STATE_NORMAL);
  }
  //delay(3);
  if (isTimeout(1000)) { //Wait fild password 30 second
    Serial.println(F(""));
    Serial.println(F("Timeout wait tracking card!"));
    lcdPrint("Timeout!", "Tracking card");
    clearData();
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}

/**
  State 4:  Wait remaining rfid card.
            The sytem design for 2 rfid (Car and Driver) .
            Must wait 2 data befor send to node mcu.
            Wait 30 second.
*/
void waitRemainingcard() {
  if (rfidListener()) {
    readCardRfid();
  }
  if (isTimeout(1000)) { //Wait 30 second.
    Serial.println(F(""));
    Serial.println(F("Timeout wait remaining card!"));
    lcdPrint("Timeout!", "Tracking card");
    clearData();
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}
/**
   State 5: Send data to server.
            Wait response from server
*/
void printResult(int result) {
  if (result == 200) {
    Serial.print(F("Check in Success!"));
    lcdPrint("Check in", "Success!");
    onBuzzer(200,2);
  } else if (result == 201) {
    Serial.print(F("Check out Success!"));
    lcdPrint("Check out", "Success!");
    onBuzzer(200,2);
  } else if (result == 400) {
    Serial.print(F("Bad request"));
    onBuzzer(200,3);
    lcdPrint("Error", "Bad request!");
  } else if (result == 500) {
    onBuzzer(200,3);
    Serial.print(F("Server Error"));
    lcdPrint("Error", "Server Error");
  }
}
void waitResponse() {
  int result = 0;
  while (ArduinoSerial.available() > 0) {
    timer = 0;
    result = ArduinoSerial.parseInt();
    if (ArduinoSerial.read() == '\n') {
      Serial.print(F("Result: ")); Serial.println(result);
    }
  }

  if (result != 0) {
    printResult(result);
    clearData();
    delay(5000);
    setState(STATE_NORMAL);
    return;
  }

  delay(30);
  if (isTimeout(1000)) { //Wait 30 second.
    Serial.println(F(""));
    Serial.println(F("Timeout Server not response!"));
    lcdPrint("Timeout!", "Not response");
    clearData();
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}
/**
   State 6: Check in out with keypad.
    carData = "";
    driverData = "";
    input = "";
    timer = 0;
*/
void printLcdCheclInOut() {
  if (carData == "") {//Check car data first.
    lcdPrint("Enter Car Id", input);
  } else if (driverData == "") {
    lcdPrint("Enter you Id", input);
  }
}
void checkInoutWithKeypad() {
  char key = keypad.getKey();
  if (key ) {
    onBuzzer(100,1);
    timer = 0; //Whwn pressed keypad well be reset timer.
    if (key == '#') {

      if (carData == "") {
        carData = input;
        input = "";
        printLcdCheclInOut();
      } else if (driverData = "") {
        driverData = input;
        input = "";
      }

      if (carData != "" && driverData != "") {
        if ((carData.charAt(0) == 'C') && (carData.length() == 6) &&
            (driverData.charAt(0) == 'D') && (driverData.length() == 6)) {
          //Send data to node mcu.
          lcdPrint(("Car: " + carData), ("Driver: " + driverData)); //C00123,D12345
          String postData = (carData + "," + driverData);
          Serial.println("Post Data: " + postData);
          delay(10);
          ArduinoSerial.print(postData);
          setState(STATE_WAIT_RESPONSE);
        } else {
          Serial.println(F("Id card Incorrect!"));
          lcdPrint("Id Incorrect!");
          clearRfid();
          clearData();
          delay(2000);
          setState(STATE_NORMAL);
          return;
        }
      }
    } else {
      input += String::String(key);
      Serial.print(F("Data: ")); Serial.println(input);
      printLcdCheclInOut();
    }
  }

  delay(30);
  if (isTimeout(1000)) { //Wait fild password 30 second
    Serial.println(F(""));
    Serial.println(F("Timeout wait check in out with keypad!"));
    lcdPrint("Timeout!", "Wait keypad");
    onBuzzer(200,3);
    delay(2000);
    setState(STATE_NORMAL);
  }
}
void initialKeyForRfid() {
  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //Serial.print(F("Using key (for A and B):"));
  //dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  //Serial.println();
}
void initilLcd() {
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

/**
  Duration in millisec.
*/
void onBuzzer(int duration, int count) {
  if (count <= 0) {
    return;
  }
  
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER, LOW);
    delay(duration);
    digitalWrite(BUZZER, HIGH);
    delay(duration);
  }
  //digitalWrite(BUZZER, LOW);
}
//---------------------------------------------------------------------------------------------------------------
void setup() {
  pinMode(BUZZER, OUTPUT);

  Serial.begin(115200);     // Initialize serial communications with the PC
  while (!Serial);          // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  ArduinoSerial.begin(115200);

  SPI.begin();              // Init SPI bus
  rfid.PCD_Init();          // Init MFRC522

  initialKeyForRfid();
  initilLcd();
  clearData();
  digitalWrite(BUZZER, HIGH);
  Serial.println(F("Start..."));

  setState(0);
}
void loop() {
  if (STATE == STATE_NORMAL) {
    keypadListener();
    if (rfidListener()) {
      readCardRfid();
    }
  } else if (STATE == STATE_LOGIN) {
    loginToSystem();
  } else if (STATE == STATE_CRAETE_CARD) {
    fillInDataForWriteRfidCard();
  } else if (STATE == STATE_WRITE_CARD) {
    writeDatatoRfidCard();
  } else if (STATE == STATE_WAIT_CARD) {
    waitRemainingcard();
  } else if (STATE == STATE_WAIT_RESPONSE) {
    waitResponse();
  } else if (STATE == STATE_CHECK_IN_OUT_WITH_KEYPAD) {
    checkInoutWithKeypad();
  }
}
