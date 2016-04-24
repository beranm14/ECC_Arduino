#include "ECDH.h"
#include "key.h"
#include "aes.h"
//====================
unsigned char Ka[KA_SIZE];

Point P;
Point Key;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  randomSeed(millis());
  initialECDH(&P, Ka);
  loadKey(Key.x,30);
  loadKey(Key.y,10);
}

void loop() {
  unsigned char  tmp[SIZE];
  unsigned char  wrd[SIZE];
  if (Serial.available() > 0) {
      byte keyk = Serial.read();
      if(keyk == 0x10){ // Count first secret
        Point tmpA;
        copyPoly(tmpA.x, P.x);
        copyPoly(tmpA.y, P.y);
        doubleAndAdd(Ka, &tmpA);
        for(byte i = 0; i < 10; i++)
          Serial.write(tmpA.x[i]);
        for(byte i = 0; i < 10; i++)
          Serial.write(tmpA.y[i]);
      }else if(keyk == 0x11){ // Count secret key
        Point tmpB;
        cleanPoly(tmpB.x);
        cleanPoly(tmpB.y);
        byte cn = 0;
        while (cn < 10){
          if (!Serial.available())
            continue;
          tmpB.x[cn] = Serial.read();   
          cn ++;
        }
        cn = 0;
        while (cn < 10){
          if (!Serial.available())
            continue;
          tmpB.y[cn] = Serial.read();
          cn ++;
        }
        for(byte i = 0; i < 10; i++)
          Serial.write(tmpB.x[i]);
        for(byte i = 0; i < 10; i++)
          Serial.write(tmpB.y[i]);
        doubleAndAdd(Ka, &tmpB);
        copyPoly(Key.x,tmpB.x);
        copyPoly(Key.y,tmpB.y);
        Serial.write(0x34);
      }else if(keyk == 0x21){
        byte cn = 0;
        while (cn < 16){
          if (!Serial.available())
            continue;
          wrd[cn] = Serial.read();
          cn ++;           
        }
        unsigned char key[176];
        for(unsigned char j = 0; j < 10; j ++)
          key[j] = Key.x[j];
        for(unsigned char j = 10; j < 16; j ++)
          key[j] = 0;
        AES_encrypt_128(wrd, key, tmp);
        for(byte i = 0; i < 16; i++)
          Serial.write(wrd[i]);
      }else if(keyk == 0x22){
        byte cn = 0;
        while (cn < 16){
          if (!Serial.available())
            continue;
          wrd[cn] = Serial.read();
          cn ++;
        }
        unsigned char key[176];
        for(unsigned char j = 0; j < 10; j ++)
          key[j] = Key.x[j];
        for(unsigned char j = 10; j < 16; j ++)
          key[j] = 0;
        AES_decrypt_128(wrd, key, tmp);
        for(byte i = 0; i < 16; i++)
          Serial.write(wrd[i]);
      }else if(keyk == 0x60){ // save keys
        saveKey(Key.x,30);
        saveKey(Key.y,10);
        Serial.write(0x60);
      }else if(keyk == 0x33){ // juck ack
        Serial.write(0x33);
      }
   }
}
