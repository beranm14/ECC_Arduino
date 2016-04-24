#include "key.h"
#include <EEPROM.h>


void loadKey(unsigned char * key, unsigned int from){
  for(int i = 0; i < 16; i ++){
    key[i] = EEPROM.read(i+from); 
  }  
}

void saveKey(unsigned char * key, unsigned int from){
  for(int i = 0; i < 16; i ++){
    EEPROM.write(i+from,key[i]); 
  }
}
