/**
 * passduino v0.2 
 * memory structure :
 * we can have 5 users with 20 password for user
 * 
 * 4 bytes at each section for ID 
 * 10 bytes for each password 
 * 
 * total EEPROM used 
 * 4*5 = 20
 * 9*20*5=900     // 9 chars for each password and 20 passwords for 5 users 
 * so in total 920 and we still have 104  bytes that can be used for bruteforce protection and for storing masterpassword 
 * 
 */
#include <avr/pgmspace.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#define SS_PIN 10 //for mifare reader
#define RST_PIN 9 // for mifare reader
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[3];

//init increment for all loops 
byte i;

//String for serial data between frontend  and backend
String SerialData;
char SerialBuffer[10];
//Array for admin password
 char AdminPassword[10];
//Bool for keeping the device in Admin Mode and stop scanning for Cards
bool AdminMode;
// byte to choose between different mods of operations in Admin mode
byte Mode[2];
//Memory Addresses for each sections
 const PROGMEM int user1Memory=0; // id associated to user is stored in the first 4 bytes 
 const PROGMEM int user2Memory=184;
 const PROGMEM int user3Memory=368;
 const PROGMEM int user4Memory=552;
 const PROGMEM int user5Memory=736;
 //dummy variable
 int dummy;
 //function definitions:
/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize);

/**
 * checking for cards and save the NUID  into the nuidPICC array
 */
void checkForCards();


void setup() { 
  Serial.begin(19200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  AdminMode = false;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  while(!Serial)
  {
    // wait for serial port
  }
  //getting AdminPassword From EEPROM
  EEPROM.get(921,AdminPassword);
}



 
void loop() 
{
  if(Serial.available() > 0)
  {
   SerialData=Serial.readStringUntil('\n');
   SerialData.getBytes(SerialBuffer,10);
   if (strcmp(AdminPassword,SerialBuffer)==0)
    {
    //starting Admin Mode
     Serial.print('1');
     AdminMode = true;
     while (AdminMode)
     {
        if(Serial.available() > 0)
         {
          SerialData="";
          //could be implemented better !!! but just for the notte dei ricercatori
          SerialData=Serial.readStringUntil('\n');
          SerialData.getBytes(Mode,2);
          //check for correct modes 
          if(Mode[0] != 49 && Mode[0] != 50 && Mode[0] != 51 )
           {
            Serial.println("Protocol Error !!!");
            AdminMode=false;
            break;
           }
          
           while(Mode[0] == 49)
           //change device master password
           {
             if(Serial.available() > 0)
                    {
                      SerialData="";
                      SerialData=Serial.readStringUntil('\n');
                      SerialData.getBytes(SerialBuffer,10);
                      EEPROM.put(921,SerialBuffer); // update user password
                      EEPROM.get(921,AdminPassword);
                      Mode[0] = 0;
                      AdminMode=false;
                      break;      
                    }
              }
          
          
          }
         }
      }
   else
    {
     Serial.print('0');
     AdminMode=false;
    }
  } 
  checkForCards();
}





void checkForCards()
{
   // Look for new cards
  if (!rfid.PICC_IsNewCardPresent())
    return;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    return;
    // Store NUID into nuidPICC array
   for ( i = 0; i < 4; i++) 
   {
      nuidPICC[i] = rfid.uid.uidByte[i];
   }
    Serial.println("The NUID tag is:");
    printHex(rfid.uid.uidByte,MFRC522::MF_KEY_SIZE); 
    Serial.println();
    delay(500);
}



void printHex(byte *buffer, byte bufferSize) {
  for (i= 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


