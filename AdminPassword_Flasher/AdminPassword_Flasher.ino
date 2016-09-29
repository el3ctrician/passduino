
#include <EEPROM.h>
char AdminPassword[10]={'p','a','s','s','w','o','r','d'};
void setup() {

  Serial.begin(19200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("flashing AdminPassword");

  EEPROM.put(921, AdminPassword);
  
  Serial.println("Done");
}

void loop() {
  /* Empty loop */
}
