
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
//string for admin password
String AdminPassword="Password";
//Bool for keeping the device in Admin Mode and stop scanning for Cards
bool AdminMode;
// byte to choose between different mods of operations in Admin mode
byte Mode;
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
}



 
void loop() 
{
  if(Serial.available() > 0)
  {
   SerialData=Serial.readStringUntil('\n');
   if (SerialData == AdminPassword)
    {
    //starting Admin Mode
     Serial.print('1');
     AdminMode = true;
     while (AdminMode)
     {
      while(!Serial.available() > 0)
      {
        if(Serial.available() > 0)
         {
          Mode=Serial.read();
          switch (Mode)
          {
           //change device master password
           case 1:
             while(!Serial.available() > 0)
              {
                if(Serial.available() > 0)
                    {
                      AdminPassword=Serial.readStringUntil('\n');  
                      AdminMode=false;
                      break;      
                    }
              }
           break;      
           case 2:
            //aggiungi un ID e password
           break;
           case 3:
           //modificare password associata ad un TAG
           break;
           default:
            // give error
           break;
          }
          break;

         }
      }
      AdminMode=false; //ending Admin Mode
     }
    }
   else
    {
     Serial.print('0');
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


