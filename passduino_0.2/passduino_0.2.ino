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

// Init array that will store new NUID (needed by MFRC552 lib ) 
byte nuidPICC[4];
//variable to store/search for tag ids in memory
byte TagId[3];
//init increment for all loops 
byte i;

//String for serial data between frontend  and backend
String SerialData;
char SerialBuffer[9];
//Array for dumping passwords from memory
char PasswordBuffer[9];
//Array for admin password
 char AdminPassword[9];
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
//users ids saved
byte user1Id[4];
byte user2Id[4];
byte user3Id[4];
byte user4Id[4];
byte user5Id[4];
//a byte to count users saved on device phisically on addr 1023 
byte ActiveUsers;
//function definitions
/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize);

/**
 * checking for cards and save the NUID  into the nuidPICC array
 */
bool checkForCards();

/**
 * to search internal EEPROM for a card id and extract the password saving it to PasswordBuffer
 */
bool getSavedPassword();

//start setup
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
  //getting user ids
  EEPROM.get(user1Memory,user1Id);
  EEPROM.get(user2Memory,user2Id);
  EEPROM.get(user3Memory,user3Id);
  EEPROM.get(user4Memory,user4Id);
  EEPROM.get(user5Memory,user5Id);
}



 
void loop() 
{
  if(Serial.available() > 0)
  {
   SerialData=Serial.readStringUntil('\n');
   SerialData.getBytes(SerialBuffer,9);
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
          if(Mode[0] != 49 && Mode[0] != 50 && Mode[0] != 51 && Mode[0] != 52 )
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
                      SerialData.getBytes(SerialBuffer,9);
                      EEPROM.put(921,SerialBuffer); // update user password
                      EEPROM.get(921,AdminPassword);
                      Serial.println('1');
                      Mode[0] = 0;
                      AdminMode=false;
                      break;      
                    }
             } 
           while(Mode[0] == 50)
           //add tag ID and password
           {
            //init the id variable and remove any previous cards value
            for (byte i = 0; i < 4; i++) 
            {
              nuidPICC[i] = 0x00;
            }
            while(!checkForCards())
            {
             //wait for new card to be detected
            }
            EEPROM.put(user1Memory,nuidPICC);
            Serial.println('1');
            while (Serial.available() <= 0 )
            {  
            }
            if(Serial.available() > 0)
            {
             SerialData="";
             SerialData=Serial.readStringUntil('\n');
             SerialData.getBytes(SerialBuffer,9);
             EEPROM.put(user1Memory+4,SerialBuffer); // update user password
             EEPROM.get(user1Memory,user1Id);  //update new userid
             Serial.println('1');
             Mode[0] = 0;
             AdminMode=false;      
            }
           
            } //end of mode 50
            while(Mode[0] == 51)
           //change password for a specific tag
           {
            //init the id variable and remove any previous cards value
            for (byte i = 0; i < 4; i++) 
            {
              nuidPICC[i] = 0x00;
            }
            while(!checkForCards())
            {
             //wait for new card to be detected
            }
            if (user1Id[0] == nuidPICC[0] && 
                user1Id[1] == nuidPICC[1] && 
                user1Id[2] == nuidPICC[2] && 
                user1Id[3] == nuidPICC[3] ) 
              {
               Serial.println('1');
               while (Serial.available() <= 0 )
               {
                //wait for Serial commands  
               }
               if(Serial.available() > 0)
               {
                SerialData="";
                SerialData=Serial.readStringUntil('\n');
                SerialData.getBytes(SerialBuffer,9);
                EEPROM.put(user1Memory+4,SerialBuffer); // update user password
                Serial.println('1');
                Mode[0] = 0;
                AdminMode=false;
                break;  
               }
              }
            if (user2Id[0] == nuidPICC[0] && 
                user2Id[1] == nuidPICC[1] && 
                user2Id[2] == nuidPICC[2] && 
                user2Id[3] == nuidPICC[3] ) 
              {
               Serial.println('1');
               while (Serial.available() <= 0 )
               {
                //wait for Serial commands  
               }
               if(Serial.available() > 0)
               {
                SerialData="";
                SerialData=Serial.readStringUntil('\n');
                SerialData.getBytes(SerialBuffer,9);
                EEPROM.put(user2Memory+4,SerialBuffer); // update user password
                Serial.println('1');
                Mode[0] = 0;
                AdminMode=false;
                break;  
               }
              }
            if (user3Id[0] == nuidPICC[0] && 
                user3Id[1] == nuidPICC[1] && 
                user3Id[2] == nuidPICC[2] && 
                user3Id[3] == nuidPICC[3] ) 
              {
               Serial.println('1');
               while (Serial.available() <= 0 )
               {
                //wait for Serial commands  
               }
               if(Serial.available() > 0)
               {
                SerialData="";
                SerialData=Serial.readStringUntil('\n');
                SerialData.getBytes(SerialBuffer,9);
                EEPROM.put(user3Memory+4,SerialBuffer); // update user password
                Serial.println('1');
                Mode[0] = 0;
                AdminMode=false;
                break;  
               }
              }
            if (user4Id[0] == nuidPICC[0] && 
                user4Id[1] == nuidPICC[1] && 
                user4Id[2] == nuidPICC[2] && 
                user4Id[3] == nuidPICC[3] ) 
              {
               Serial.println('1');
               while (Serial.available() <= 0 )
               {
                //wait for Serial commands  
               }
               if(Serial.available() > 0)
               {
                SerialData="";
                SerialData=Serial.readStringUntil('\n');
                SerialData.getBytes(SerialBuffer,9);
                EEPROM.put(user4Memory+4,SerialBuffer); // update user password
                Serial.println('1');
                Mode[0] = 0;
                AdminMode=false; 
                break; 
               }
              }
            if (user5Id[0] == nuidPICC[0] && 
                user5Id[1] == nuidPICC[1] && 
                user5Id[2] == nuidPICC[2] && 
                user5Id[3] == nuidPICC[3] ) 
              {
               Serial.println('1');
               while (Serial.available() <= 0 )
               {
                //wait for Serial commands  
               }
               if(Serial.available() > 0)
               {
                SerialData="";
                SerialData=Serial.readStringUntil('\n');
                SerialData.getBytes(SerialBuffer,9);
                EEPROM.put(user5Memory+4,SerialBuffer); // update user password
                Serial.println('1');
                Mode[0] = 0;
                AdminMode=false;
                break;  
               }
              }
            Serial.println('0');
            Mode[0] = 0;
            AdminMode=false; 
            } //end of mode 51
           
           while(Mode[0] == 52)
           //revocke card and delete passwords
           {
            //init the id variable and remove any previous cards value
            for (byte i = 0; i < 4; i++) 
            {
              nuidPICC[i] = 0x00;
            }
            while(!checkForCards())
            {
             //wait for new card to be detected
            }
            if (user1Id[0] == nuidPICC[0] && 
                user1Id[1] == nuidPICC[1] && 
                user1Id[2] == nuidPICC[2] && 
                user1Id[3] == nuidPICC[3] ) 
              {
              for (byte i = 0; i < 4; i++) 
               {
                nuidPICC[i] = 0x00; //prepare a vector of 0x00 to fill the saved ID
               }
               EEPROM.put(user1Memory,nuidPICC); //store 0x00 in the tag id to deleted the previous ID
              for (byte i = 0; i < 9; i++) 
               {
                SerialBuffer[i] = 0x00; //prepare a vector of 0x00 to fill the saved password
               } 
               EEPROM.put(user1Memory+4,SerialBuffer); // delete saved passwords
               Serial.println('1');
               Mode[0] = 0;
               AdminMode=false; 
               break;
              }
            if (user2Id[0] == nuidPICC[0] && 
                user2Id[1] == nuidPICC[1] && 
                user2Id[2] == nuidPICC[2] && 
                user2Id[3] == nuidPICC[3] ) 
              {
              for (byte i = 0; i < 4; i++) 
               {
                nuidPICC[i] = 0x00; //prepare a vector of 0x00 to fill the saved ID
               }
               EEPROM.put(user2Memory,nuidPICC); //store 0x00 in the tag id to deleted the previous ID
              for (byte i = 0; i < 9; i++) 
               {
                SerialBuffer[i] = 0x00; //prepare a vector of 0x00 to fill the saved password
               } 
               EEPROM.put(user2Memory+4,SerialBuffer); // delete saved passwords
               Serial.println('1');
               Mode[0] = 0;
               AdminMode=false; 
               break;         
              }
            if (user3Id[0] == nuidPICC[0] && 
                user3Id[1] == nuidPICC[1] && 
                user3Id[2] == nuidPICC[2] && 
                user3Id[3] == nuidPICC[3] ) 
              {
              for (byte i = 0; i < 4; i++) 
               {
                nuidPICC[i] = 0x00; //prepare a vector of 0x00 to fill the saved ID
               }
               EEPROM.put(user3Memory,nuidPICC); //store 0x00 in the tag id to deleted the previous ID
              for (byte i = 0; i < 9; i++) 
               {
                SerialBuffer[i] = 0x00; //prepare a vector of 0x00 to fill the saved password
               } 
               EEPROM.put(user3Memory+4,SerialBuffer); // delete saved passwords
               Serial.println('1');
               Mode[0] = 0;
               AdminMode=false; 
               break;       
              }
            if (user4Id[0] == nuidPICC[0] && 
                user4Id[1] == nuidPICC[1] && 
                user4Id[2] == nuidPICC[2] && 
                user4Id[3] == nuidPICC[3] ) 
              {
              for (byte i = 0; i < 4; i++) 
               {
                nuidPICC[i] = 0x00; //prepare a vector of 0x00 to fill the saved ID
               }
               EEPROM.put(user4Memory,nuidPICC); //store 0x00 in the tag id to deleted the previous ID
              for (byte i = 0; i < 9; i++) 
               {
                SerialBuffer[i] = 0x00; //prepare a vector of 0x00 to fill the saved password
               } 
               EEPROM.put(user4Memory+4,SerialBuffer); // delete saved passwords
               Serial.println('1');
               Mode[0] = 0;
               AdminMode=false; 
               break;
              }
            if (user5Id[0] == nuidPICC[0] && 
                user5Id[1] == nuidPICC[1] && 
                user5Id[2] == nuidPICC[2] && 
                user5Id[3] == nuidPICC[3] ) 
              {
              for (byte i = 0; i < 4; i++) 
               {
                nuidPICC[i] = 0x00; //prepare a vector of 0x00 to fill the saved ID
               }
               EEPROM.put(user5Memory,nuidPICC); //store 0x00 in the tag id to deleted the previous ID
              for (byte i = 0; i < 9; i++) 
               {
                SerialBuffer[i] = 0x00; //prepare a vector of 0x00 to fill the saved password
               } 
               EEPROM.put(user5Memory+4,SerialBuffer); // delete saved passwords
               Serial.println('1');
               Mode[0] = 0;
               AdminMode=false; 
               break;
              }
            Serial.println('0');
            Mode[0] = 0;
            AdminMode=false; 
            } //end of mode 52            
          }//end of admin serial check
         } //end of admin mode
      }
   else
    {
     Serial.print('0');
     AdminMode=false;
    }
  }
   
  if(checkForCards())
  {
  getSavedPassword();
  }
  
 }


bool getSavedPassword()
{
  //check for first user
 if (user1Id[0] == nuidPICC[0] && 
     user1Id[1] == nuidPICC[1] && 
     user1Id[2] == nuidPICC[2] && 
     user1Id[3] == nuidPICC[3] ) 
      {
        EEPROM.get(user1Memory+4,PasswordBuffer);
        Serial.println(PasswordBuffer);
        delay(500);
        return true;
      }
  //check for second user
 if (user2Id[0] == nuidPICC[0] && 
     user2Id[1] == nuidPICC[1] && 
     user2Id[2] == nuidPICC[2] && 
     user2Id[3] == nuidPICC[3] ) 
      {
        EEPROM.get(user2Memory+4,PasswordBuffer);
        Serial.println(PasswordBuffer);
        delay(500);
        return true;
      }
  //check for third user
 if (user3Id[0] == nuidPICC[0] && 
     user3Id[1] == nuidPICC[1] && 
     user3Id[2] == nuidPICC[2] && 
     user3Id[3] == nuidPICC[3] ) 
      {
        EEPROM.get(user3Memory+4,PasswordBuffer);
        Serial.println(PasswordBuffer);
        delay(500);
        return true;
      }
    //check for forth user
 if (user4Id[0] == nuidPICC[0] && 
     user4Id[1] == nuidPICC[1] && 
     user4Id[2] == nuidPICC[2] && 
     user4Id[3] == nuidPICC[3] ) 
      {
        EEPROM.get(user4Memory+4,PasswordBuffer);
        Serial.println(PasswordBuffer);
        delay(500);
        return true;
      }
  //check for last user
 if (user5Id[0] == nuidPICC[0] && 
     user5Id[1] == nuidPICC[1] && 
     user5Id[2] == nuidPICC[2] && 
     user5Id[3] == nuidPICC[3] ) 
      {
        EEPROM.get(user1Memory+5,PasswordBuffer);
        Serial.println(PasswordBuffer);
        delay(500);
        return true;
      }
 return false;
}


bool checkForCards()
{
   // Look for new cards
  if (!rfid.PICC_IsNewCardPresent())
    return false;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return false;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    return false;
    // Store NUID into nuidPICC array
   for ( i = 0; i < 4; i++) 
   {
      nuidPICC[i] = rfid.uid.uidByte[i];
   }
   
   return true;
}



void printHex(byte *buffer, byte bufferSize) 
{
  for (i= 0; i < bufferSize; i++) 
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


