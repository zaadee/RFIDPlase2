/*
   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

//RFID-RC522 Shield-------------------------
#include <SPI.h>
#include <MFRC522.h>

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

//------------------------------------------
//Keypad -----------------------------------
#include <Keypad.h>
const byte ROWS = 4;        // Four rows
const byte COLS = 4;        //  columns
byte rowPins[4] = { A1, A0, 7, 6 } ;
byte colPins[4] = { 5, 4, 3, 2, };
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
String input; //Value when keypad.
//------------------------------------------
//Valiable----------------------------------

//LED_PIN
//#define LED_GREEN A2
//#define LED_YELLOW A3
//#define LED_RED A4
//Status off working  0 = normal state
//                    1 = Access to system.
byte state;

//Functions!!!
//---------------------------------------------------------------------------------------------------------------
void  dumpFRIDInfo() {
  // Dump debug info about the card; PICC_HaltA() is automatically called
  rfid.PICC_DumpToSerial(&(rfid.uid));
}
//---------------------------------------------------------------------------------------------------------------
void clearRfid() {
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
//---------------------------------------------------------------------------------------------------------------
void readCardRfid() {
  Serial.println(F("Read card"));

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    clearRfid();
    return;
  }

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  rfid.PICC_DumpMifareClassicSectorToSerial(&(rfid.uid), &key, sector);
  Serial.println();

  // Read data from the block
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    clearRfid();
    return;
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();
  Serial.println();
  clearRfid();
  delay(2000);
}
//---------------------------------------------------------------------------------------------------------------
void writeCardRfid() {
  Serial.println(F("Write data to card"));

  //byte dataBlock[]    = {
  //  0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
  //  0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
  //  0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
  //  0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
  //};

  // Authenticate using key B
  Serial.println(F("Authenticating again using key A..."));
  status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
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
    clearRfid();
    return;
  }
  Serial.println();
  Serial.println(F("Write data success"));
  clearRfid();
  delay(2000);
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
  //clearRfid();
  return true;

  //writeCardRfid();
  //readCardRfid();
  //delay(2000);
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
void setup() {
  //pinMode (LED_GREEN, OUTPUT);
  //pinMode (LED_YELLOW, OUTPUT);
  //pinMode (LED_RED, OUTPUT);
  //digitalWrite(LED_GREEN, LOW);
  //digitalWrite(LED_YELLOW, LOW);
  //digitalWrite(LED_RED, LOW);

  Serial.begin(9600);     // Initialize serial communications with the PC
  while (!Serial);        // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();            // Init SPI bus
  rfid.PCD_Init();        // Init MFRC522

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //Serial.print(F("Using key (for A and B):"));
  //dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  //Serial.println();

  state = 0;
  Serial.println(F("Start..."));
  Serial.println(F("State 0: Listener Keypad and Rfid"));
}
//---------------------------------------------------------------------------------------------------------------
void keypadListener() {
  char key = keypad.getKey();
  if (key ) {
    if (key == '*') {
      state = 1;
      input = ""; //Clear in put
      printSerialEnd();
      Serial.println(F("State 1: Login to system"));
      Serial.println(F("Enter password. Press # to next"));
    } else {
      Serial.println(F("Pressed * to Login"));
      delay(2000);
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
    if (key == '#') {
      if (input == pass) { //Login success. To next state fill in data for write rfid card.
        Serial.println(F("Login success!"));
        printSerialEnd();
        state = 2;
        input = "";
        Serial.println(F("State 2: Fill in data for write Rfid card"));
        Serial.println(F("Enter card data. Press # to next"));
      } else { //password incorrect. Set state to default (0), Clear in put and delay 2 second.
        Serial.println(F("Password Incorrect!"));
        printSerialEnd();
        Serial.println(F("State 0: Listener Keypad and Rfid"));
        state = 0;
        input = "";
        delay(2000);
      }
    } else {
      input += String::String(key);
      Serial.print(F("Password: ")); Serial.println(input) ;
    }
  }
}
//---------------------------------------------------------------------------------------------------------------
/**
  State 2:  Fill in data for write Rfid Card.
            Enter data for write to rfid RFID card.
            Data type car enter       C00001, C00002.... (Can stor at 99,999 user)
            Data type employee enter  D00001, D00002.... (Can stor at 99,999 user)
*/
byte getBuyte5555(char key) {
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
    if (key == '#') {
      if (input.length() > 0) { //If have data. to next state Write data to Rfid card.
        state = 3;
        Serial.println(F("Data in array: "));
        dump_byte_array(dataBlock, 16); Serial.println();
        printSerialEnd();
        Serial.println(F("State 3: Write data to Rfid card"));
        Serial.println(F("Please Tracking Rfid card for write data..."));
      } else { //not input data return to new state 2.
        Serial.println(F("Please Enter Data!."));
        printSerialEnd();
        Serial.println(F("State 2: Write data to Rfid card"));
        Serial.println(F("Enter card data. Press # to next"));
      }
    } else {
      input += String::String(key);
      if ((input.length() - 1) < 16) {
        dataBlock[(input.length() - 1)] = getBuyte5555(key);
      }
      Serial.print(F("Data: ")); Serial.println(input);
    }
  }
}
//---------------------------------------------------------------------------------------------------------------
/**
  State 3:  Write data to Rfid Card.
            Tracking Rfid card for write (Variable: input)
*/
void writeDatatoRfidCard() {
  if (rfidListener()) {
    writeCardRfid();
    printSerialEnd();
    state = 0;
    Serial.println(F("State 0: Listener Keypad and Rfid"));
  }
}
//---------------------------------------------------------------------------------------------------------------
void printSerialEnd() {
  Serial.println(F("--------------------------------------"));
}
void loop() {
  if (state == 0) {
    keypadListener();
    if (rfidListener()) {
      readCardRfid();
    }
  } else if (state == 1) {
    loginToSystem();
  } else if (state == 2) {
    fillInDataForWriteRfidCard();
  } else if (state == 3) {
    writeDatatoRfidCard();
  }

  //delay(200);
}
