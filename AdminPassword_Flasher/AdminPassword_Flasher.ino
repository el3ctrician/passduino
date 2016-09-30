

/**
 * a simple program to flash default master passwrod and clear the active users counter
 */
#include <EEPROM.h>
char AdminPassword[10]={'p','a','s','s','w','o','r','d'};
byte ActiveUsers = 0x00;
void setup() {

  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("flashing AdminPassword");

  EEPROM.put(921, AdminPassword);
  EEPROM.put(1023,ActiveUsers);
  Serial.println("Done");
}

void loop() {
  /* Empty loop */
}
