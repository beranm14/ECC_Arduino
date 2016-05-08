#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "rs232.h"
#define SIZE ((20))
#define HALFSIZE ((10))

using namespace std;

//a = 4A2E 38A8F66D 7F4C385F
unsigned char POLA[SIZE] = { 0x5F, 0x38, 0x4C, 0x7F, 0x6D, 0xF6, 0xA8, 0x38, 0x2E, 0x4A };
//b = 2C0B B31C6BEC C03D68A7
unsigned char POLB[SIZE] = { 0xA7, 0x68, 0x3D, 0xC0, 0xEC, 0x6B, 0x1C, 0xB3, 0x0B, 0x2C };
//
unsigned char x_P[SIZE] = { 0x0F, 0xF1, 0x92, 0x27, 0xE4, 0x63, 0x7B, 0x12, 0xCB, 0x30 };
unsigned char y_P[SIZE] = { 0x3B, 0x71, 0x4F, 0xB0, 0x6B, 0x26, 0x88, 0x2C, 0x7B, 0x54 };
/*unsigned char x_Q[SIZE] = { 0x25, 0x73, 0x49, 0x14, 0x50, 0x03, 0x9F, 0x2A, 0x20, 0x00 };
unsigned char y_Q[SIZE] = { 0x29, 0xC1, 0x97, 0x2F, 0x55, 0x59, 0x48, 0xA6, 0x75, 0x51 };*/
/*unsigned char x_P[SIZE];
unsigned char y_P[SIZE];
unsigned char x_Q[SIZE];
unsigned char y_Q[SIZE];*/
//f = α^79 + α^9 + 1
unsigned char p[SIZE] = { 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };

unsigned char  I_x[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  I_y[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define ROTL8(x,shift) ((unsigned char) ((x) << (shift)) | ((x) >> (8 - (shift)))) // wiki

//
void rotate(unsigned char *in) {
        unsigned char a,c;
        a = in[0];
        for(c=0;c<3;c++)
                in[c] = in[c + 1];
        in[3] = a;
        return;
}

//
unsigned char rcon(unsigned char in) {
        unsigned char c=1;
        if(in == 0)
                return 0;
        while(in != 1) {
    unsigned char b;
    b = c & 0x80;
    c <<= 1;
    if(b == 0x80) {
      c ^= 0x1b;
    }
                in--;
        }
        return c;
}

// wiki
unsigned char sbox(unsigned char p_get) {
  /* loop invariant: p * q == 1 in the Galois field */
  unsigned char p = 1, q = 1, tmp;
  do {
    /* multiply p by x+1 */
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
    /* divide q by x+1 */
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;
    /* compute the affine transformation */
    tmp = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    //sbox[p] = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    if(p_get == p){
        return tmp;
    }
  } while (p != 1);
  /* 0 is a special case since it has no inverse */
  return 0x63;
}

unsigned char sbox_inv(unsigned char p_get) {
  /* loop invariant: p * q == 1 in the Galois field */
  unsigned char p = 1, q = 1, tmp;
  do {
    /* multiply p by x+1 */
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
    /* divide q by x+1 */
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;
    /* compute the affine transformation */
    tmp = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    //sbox[p] = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    //isbox[sbox[p]] = p;
    //isbox[tmp] = p;
    if(tmp == p_get){
        return p;
    }
  } while (p != 1);
  /* 0 is a special case since it has no inverse */
  return 0;
}

/*
void sbox_field(unsigned char * fild,unsigned char * inv_fild){  unsigned char p = 1, q = 1;
  do {
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;
    fild[p] = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    //sbox[p] = 0x63 ^ q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
    inv_fild[fild[p]] = p;
  } while (p != 1);
  fild[0] = 0x63;
  inv_fild[0] = 0x52;
}
*/


void translate_matrix(unsigned char * mat){
  for(unsigned char i = 0; i < 4 ;i ++)
    for(unsigned char j = i; j < 4 ;j ++){
      if (i != j){
        unsigned char tmp = mat[i*4+j];
        mat[i*4+j] = mat[j*4+i];
        mat[j*4+i] = tmp;
      }
   }
}


// My work! beranm14
void printMatrix(unsigned char * wrd, unsigned char x, unsigned char y ){
  for(unsigned char i=0; i<x; i++){
    for(unsigned char j=0; j<y; j++){
      printf("%x",((unsigned char)wrd[(4*i)+j]));
      printf(" ");
    }
    printf("\n");
  }
}
/*
void printExpKey(unsigned char * key){
  unsigned char i = 0;
  while (i<176){
    if(i%16 == 0)
      printf("\n");
    printf(((unsigned char)key[i]),HEX);
    i++;
  }
  printf("\n");
}


void printSbox(){
  for(unsigned char y = 0; y < 16 ;y++){
    for(unsigned char x = 0; x < 16 ;x++){
      printf( sbox( ((y << 4 | x)) ),HEX);
      printf(" ");
      printf((y << 4 | x));
      printf("|");
    }
    printf("\n");
  }
}
void printInvSbox(){
  for(unsigned char y = 0; y < 16 ;y++){
    for(unsigned char x = 0; x < 16 ;x++){
      //printf(sbox_inv( (y << 4 | x) ),HEX);
      //printf(" ");
      printf( sbox_inv( (y << 4 | x) ),HEX);
      printf(" ");
      printf((y << 4 | x));
      printf("|");
    }
    printf("\n");
  }
}
*/

// My work! beranm14
void ShiftRows(unsigned char * wrd,unsigned char * tmp){
  //translate_matrix(wrd);
  for(unsigned char i=0; i<4; i++){
    for(unsigned char j=0; j<4; j++){
      tmp[(4*i)+j] = wrd[(4*((j+i)%4))+j];
    }
  }
 for(unsigned char i=0; i<16; i++){
    wrd[i] = tmp[i];
 }
 //translate_matrix(wrd);
}
void InvShiftRows(unsigned char * wrd,unsigned char * tmp){
 //translate_matrix(wrd);
 for(unsigned char i=0; i<4; i++){
    for(unsigned char j=0; j<4; j++){
      //tmp[(4*j)+(j+i)%4] = wrd[(4*j)+i];
      tmp[(4*((j+i)%4))+j] = wrd[(4*i)+j];
    }
  }
 for(unsigned char i=0; i<16; i++){
    wrd[i] = tmp[i];
 }
 //translate_matrix(wrd);
}

// Wiki
void MixFour(unsigned char * wrd){
        unsigned char a[4];
        unsigned char b[4];
        unsigned char c;
        unsigned char h;
        /* The array 'a' is simply a copy of the input array 'r'
         * The array 'b' is each element of the array 'a' multiplied by 2
         * in Rijndael's Galois field
         * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
        for(c=0;c<4;c++) {
                a[c] = wrd[c];
                /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
                h = (unsigned char)((signed char)wrd[c] >> 7); /* arithmetic right shift, thus shifting in either zeros or ones */
                b[c] = wrd[c] << 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
                b[c] ^= 0x1B & h; /* Rijndael's Galois field */
        }
        wrd[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
        wrd[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
        wrd[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
        wrd[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

unsigned char gmul(unsigned char a, unsigned char b) {
   unsigned char p = 0;
   unsigned char counter;
   unsigned char hi_bit_set;
   for (counter = 0; counter < 8; counter++) {
      if ((b & 1) != 0) {
         p ^= a;
      }
      hi_bit_set = (unsigned char) (a & 0x80);
      a <<= 1;
      if (hi_bit_set != 0) {
         a ^= 0x1b; /* x^8 + x^4 + x^3 + x + 1 */
      }
      b >>= 1;
   }
   return p;
}

void InvMixFour(unsigned char * s){
     unsigned char ss[4];
     ss[0] = (gmul(0x0e, s[0]) ^ gmul(0x0b, s[1]) ^ gmul(0x0d,s[2]) ^ gmul(0x09,s[3]));
     ss[1] = (gmul(0x09, s[0]) ^ gmul(0x0e, s[1]) ^ gmul(0x0b,s[2]) ^ gmul(0x0d,s[3]));
     ss[2] = (gmul(0x0d, s[0]) ^ gmul(0x09, s[1]) ^ gmul(0x0e,s[2]) ^ gmul(0x0b,s[3]));
     ss[3] = (gmul(0x0b, s[0]) ^ gmul(0x0d, s[1]) ^ gmul(0x09,s[2]) ^ gmul(0x0e,s[3]));
     for(unsigned char i = 0;i<4;i++)
        s[i] = ss[i];
}

void MixColumns(unsigned char * wrd){
   //translate_matrix(wrd);
    unsigned char tmp[4];
    for (unsigned char i = 0; i<4; i++){
      for (unsigned char j = 0; j<4; j++){
        tmp[j] = wrd[i*4+j];
      }
      MixFour(tmp);
      for (unsigned char j = 0; j<4; j++){
        wrd[i*4+j] = tmp[j];
      }
    }
    //translate_matrix(wrd);
}

void InvMixColumns(unsigned char * wrd){
   //translate_matrix(wrd);
    unsigned char tmp[4];
    for (unsigned char i = 0; i<4; i++){
      for (unsigned char j = 0; j<4; j++){
        tmp[j] = wrd[i*4+j];
      }
      InvMixFour(tmp);
      for (unsigned char j = 0; j<4; j++){
        wrd[i*4+j] = tmp[j];
      }
    }
   //translate_matrix(wrd);
}

//http://www.samiam.org/key-schedule.html
void schedule_core(unsigned char *in, unsigned char i) {
        unsigned char a;
        /* Rotate the input 8 bits to the left */
        rotate(in);
        /* Apply Rijndael's s-box on all 4 unsigned chars */
        for(a = 0; a < 4; a++)
                in[a] = sbox(in[a]);
        /* On just the first unsigned char, add 2^i to the unsigned char */
        in[0] ^= rcon(i);
}

//http://www.samiam.org/key-schedule.html
void expand_key(unsigned char *in) { //128 bit to another 176 unsigned chars
        unsigned char t[4];
        /* c is 16 because the first sub-key is the user-supplied key */
        unsigned char c = 16;
        unsigned char i = 1;
        unsigned char a;
        /* We need 11 sets of sixteen unsigned chars each for 128-bit mode */
        while(c < 176) {
                /* Copy the temporary variable over from the last 4-unsigned char
                 * block */
                for(a = 0; a < 4; a++)
                        t[a] = in[a + c - 4];
                /* Every four blocks (of four unsigned chars),
                 * do a complex calculation */
                if(c % 16 == 0) {
                    schedule_core(t,i);
                    i++;
                }
                for(a = 0; a < 4; a++) {
                    in[c] = in[c - 16] ^ t[a];
                    c++;
                }
        }
}

void addRoundKey(unsigned char * key, unsigned char * wrd){
  for(unsigned char i=0;i<16;i++){
    wrd[i] = wrd[i] ^ key[i];
  }
}

void InvSubByte(unsigned char * wrd){
  for (unsigned char i = 0;i<16;i++){
        /*unsigned char x = wrd[i] & 15;
        unsigned char y = wrd[i] >> 4 & 15;
        unsigned char gtxy = y*16+x;*/
        wrd[i] = sbox_inv(wrd[i]);
  }
}

void SubByte(unsigned char * wrd){
  for (unsigned char i = 0;i<16;i++){
        /*unsigned char x = wrd[i] & 15;
        unsigned char y = wrd[i] >> 4 & 15;
        unsigned char gtxy = y*16+x;
        wrd[i] = sbox(gtxy);*/
        wrd[i] = sbox(wrd[i]);
  }
}

unsigned char * AES_encrypt_128(unsigned char * wrd, unsigned char * key, unsigned char * tmp){
    // initial round
    expand_key(key);
    addRoundKey(key, wrd);
    key+=16;
    // rounds
    for(unsigned char i = 0; i<9; i++){
      // sub unsigned chars
      SubByte(wrd);
      // shift rows
      ShiftRows(wrd,tmp);
      // mix columns
      MixColumns(wrd);
      // mix with keys
      addRoundKey(key, wrd);
      key+=16;
    }
    // final round
    // sub unsigned chars
    SubByte(wrd);
    // shift rows
    ShiftRows(wrd,tmp);
    // mix with keys
    addRoundKey(key, wrd);
    //======================
    return wrd;
}
unsigned char * AES_decrypt_128(unsigned char * wrd, unsigned char * key, unsigned char * tmp){
    // initial round
    expand_key(key);
    key += 176;
    key -= 16;
    addRoundKey(key, wrd);
    key -= 16;
    // rounds
    for(unsigned char i = 0; i<9; i++){
      InvShiftRows(wrd,tmp);
      InvSubByte(wrd);
      addRoundKey(key, wrd);
      key -= 16;
      InvMixColumns(wrd);
    }
    // final round
    InvShiftRows(wrd,tmp);
    InvSubByte(wrd);
    addRoundKey(key, wrd);
    return wrd;
}



class Point{
    public:
        unsigned char x[SIZE];
        unsigned char y[SIZE];
};

inline unsigned char * add(unsigned char * a, unsigned char * b, unsigned char from = 0){ // changed
    for(unsigned char i = from; i < HALFSIZE; i++){
        a[i] = a[i] ^ b[i];
    }
    return a;
}


inline bool isNotZero(unsigned char * a){ // changed
    for(unsigned char i = 0; i < HALFSIZE; i++){
        if (a[i] != 0)
            return true;
    }
    return false;
}

inline void shiftLeft(unsigned char * a, unsigned char k = SIZE){ // changed
    unsigned int tmp = 0;
    unsigned int tmr = 0;
    for (unsigned char j = 0 ; j < k; j++){
        tmp = a[j] << 1;
        //tmp <<= 1;
        a[j] = tmp;
        if ((tmr & 0x100))
            a[j] |= 0x01;
        tmr = tmp;
    }
/*    unsigned char tmp = 0;
    for (unsigned char j = 0 ; j < k; j++){
        unsigned char tmr = a[j] & 0x80;
        a[j] <<= 1;
        if (tmp)
            a[j] |= 1;
        tmp = tmr;
    }
*/
}

inline unsigned char isZero(unsigned char * a){ //changed
    for(unsigned char i = 0; i < HALFSIZE; i++){
        if(a[i] != 0){
            return false;
        }
    }
    return true;
}

inline unsigned int getHighBit(unsigned char * a){ // changed
   /*if(isZero(a))
        return 0;*/
    unsigned int bits_size = HALFSIZE * 8;
    unsigned int l = 0;
    for (int i = HALFSIZE - 1; i != -1; i --){
        /*
        unsigned char tmp = 0x80;
        while ((a[i] & tmp) == 0 && tmp != 0){
            tmp >>= 1;
            l ++;
        }
        if(tmp != 0)
            break;
        */
        if((a[i] & 0x80)){ // vypadá to hnusně, ale v testech to ušetřilo půl sekundy
            break;
        }
        l ++;
        if((a[i] & 0x40)){
            break;
        }
        l ++;
        if((a[i] & 0x20)){
            break;
        }
        l ++;
        if((a[i] & 0x10)){
            break;
        }
        l ++;
        if((a[i] & 0x08)){
            break;
        }
        l ++;
        if((a[i] & 0x04)){
            break;
        }
        l ++;
        if((a[i] & 0x02)){
            break;
        }
        l ++;
        if((a[i] & 0x01)){
            break;
        }
        l ++;
    }
    l = bits_size - l;
    return l;
}

inline unsigned char * copyPoly(unsigned char * a, unsigned char * b, unsigned int sz = SIZE){
   memcpy(a, b, sz);
   /* for(unsigned char i = 0; i < SIZE; i++){
        a[i] = b[i];
    }*/
  return a;
}
inline void cleanPoly(unsigned char * a){
  memset(a, 0, SIZE);
  /*  for(unsigned char i = 0; i < SIZE; i++){
        a[i] = 0;
    }*/
}


/*inline unsigned char * mul(unsigned char * a, unsigned char * b, unsigned char * res){
    unsigned char tmp[SIZE];
    copyPoly(tmp,a);
    for(unsigned char i = 0; i < SIZE; i++){
        unsigned char j = 1;
        while(j != 0){
            if((b[i] & j) > 0){
                add(res, tmp);
            }
            shiftLeft(tmp);
            j <<= 1;
        }
    }
    return res;
}*/
inline void addfromto(unsigned char * a, unsigned char * b, unsigned char from, unsigned char to){
    for(unsigned char i = from; i < to; i ++){
        a[i] = a[i] ^ b[i - from];
    }
}

inline unsigned char * mul(unsigned char * a, unsigned char * b, unsigned char * res){ // changed
    //unsigned char res[SIZE];
    cleanPoly(res);
    unsigned char tmpa[SIZE];
    copyPoly(tmpa, a);
    unsigned char j = 0x01;
    for(unsigned char k = 0; k < 8; k++){
        for(unsigned char i = 0; i < SIZE; i++){
            if (b[i] & j){
                addfromto(res, tmpa, i, SIZE);
            }
        }
        j <<= 1;
        shiftLeft(tmpa);
    }
    return res;
}


inline unsigned char * getPoly(unsigned int a, unsigned char * poly){ // changed
    unsigned int where = a / 8;
    unsigned int which = a % 8;
    /*poly[where] = 1;
    while (which){
        poly[where] <<= 1;
        which --;
    }*/
    if(which == 0){  // tohle neurychlilo vůbec nic
        poly[where] = 1;
    }else if(which == 1){
        poly[where] = 2;
    }else if(which == 2){
        poly[where] = 4;
    }else if(which == 3){
        poly[where] = 8;
    }else if(which == 4){
        poly[where] = 0x10;
    }else if(which == 5){
        poly[where] = 0x20;
    }else if(which == 6){
        poly[where] = 0x40;
    }else if(which == 7){
        poly[where] = 0x80;
    }
    return poly;
}

void printA(unsigned char * a){
    for(unsigned char i = 0; i < SIZE; i++){
        cout << hex << (unsigned int)a[i];
        cout << " ";
    }
    cout << endl;
}

inline void divMod(unsigned char * a, unsigned char * b, unsigned char * m, unsigned char * d){ // changed
    unsigned int  alen = getHighBit(a);
    unsigned int  blen = getHighBit(b);
    if (alen == 0 || blen == 0 || alen < blen){
        copyPoly(m, a);
        cleanPoly(d);
        return;
    }
    unsigned char tmp_a[SIZE];
    copyPoly(tmp_a, a);
    unsigned int dr;
    while( 1 ){
        unsigned char tmpl[SIZE];
        cleanPoly(tmpl);
        unsigned char tmpp[SIZE];
        cleanPoly(tmpp);
        unsigned char tmpres[SIZE];
        //cleanPoly(tmpres);
        //
        dr = (getHighBit(tmp_a) - getHighBit(b));
        //cout << (unsigned int) dr << endl;
        //return;
        add(d,getPoly(dr,tmpp));
        copyPoly(m, add(tmp_a, mul(getPoly(dr,tmpp),b,tmpres))); //*m = a ^ mulPoly(getPoly(dr),b);
        //m = add(a, mul(getPoly(dr,tmpp),b,tmpres));
        if(getHighBit(m) < blen)
            break;
        else{
            copyPoly(tmp_a, m);
        }
    }
    //copyPoly(a,tmp_a);
}

inline unsigned char isOne(unsigned char * b){ //changed
    for(unsigned char i = 1; i < HALFSIZE; i++){
        if(b[i] != 0)
            return 0;
    }
    if(b[0] == 0x01)
        return 1;
    return 0;
}


/*unsigned char * reduce(unsigned char * b){
    unsigned char m[SIZE], q[SIZE];
    divMod(b, p, m, q);
    copyPoly(b,m);
    return b;
}*/

inline unsigned char * reduce(unsigned char * c){
    unsigned char up, down, tmp, i;
    for (i = SIZE - 1; i >= SIZE/2; i--) {
        up = ((c[i] << 0x01) & 0xFE);
        down = ((c[i - 1] >> 7) & 0x01);
        tmp = (up ^ down);
        up = ((tmp >> 7) & 0x01);
        down = ((tmp << 1) & 0xFE);
        c[i - 8] ^= up;
        c[i - 9] ^= down;
        c[i - 10] ^= tmp;
    }
    c[SIZE/2 - 1] &= 0x7F;
    i = SIZE;
    down = ((c[i - 1] >> 7) & 0x01);
    for(i=SIZE/2;i<SIZE;i++)
        c[i] = 0;
    return c;
}

inline unsigned char isBiggerThanOne(unsigned char * b){ // changed
    for(unsigned char i = 1; i < HALFSIZE; i++){ // stačí půlka, polynomy jsou po redukci
        if(b[i] != 0)
            return 1;
    }
    if(b[0] > 1)
        return 1;
    return 0;
}

inline unsigned char isEqualPoly(unsigned char * a, unsigned char * b){
    return memcmp(a,b,SIZE) == 0;
    /*for(unsigned char i = 0; i < SIZE; i++){
        if(a[i] != b[i])
            return false;
    }
    return true;*/
}

inline void swappo(unsigned char * a, unsigned char * b){
    unsigned char * tmp;
    tmp = a;
    a = b;
    b = tmp;
}

inline void inverse(unsigned char * a, unsigned char * b, unsigned char * res){
    unsigned char tmp_a[SIZE];
    copyPoly(tmp_a, a);
    unsigned char tmp_b[SIZE];
    copyPoly(tmp_b, b);

	unsigned char  t[SIZE], q[SIZE];
    //copyPoly(b0,b);
    cleanPoly(q);
    unsigned char x0[SIZE];
    unsigned char x1[SIZE];
    //unsigned char * x1 = NULL;
    cleanPoly(x0);
    x0[0] = 0x01;
    cleanPoly(x1);
    unsigned char restmp[SIZE];
    unsigned char tmp[SIZE];

	if (isOne(b)) return;
	while (isBiggerThanOne(a)) {
        cleanPoly(tmp);
        cleanPoly(q);
        divMod(a, b, tmp, q);// q = a / b;
        copyPoly(t, b);// t = b;
        copyPoly(b, tmp);// b = tmp;
        copyPoly(a, t); // a = t;
        copyPoly(t, x0); //t = x0;
        cleanPoly(restmp);
        copyPoly(x0, add(x1, mul(q, x0, restmp))); // x0 = x1 + q * x0;
		//x0 = add(x1, mul(q, x0, restmp));
		copyPoly(x1, t); //x1 = t;
		//x1 = t;
    }
	copyPoly(res, x1);
	copyPoly(a,tmp_a);
    copyPoly(b,tmp_b);
}


inline unsigned char * square(unsigned char * a){ // changed
    unsigned char looksup[9] = {0x00, 0x01, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00, 0x40};
    unsigned char result[SIZE];
    unsigned char k = 0;
    cleanPoly(result);
    for(unsigned char i = 0; i < HALFSIZE; i++){
            /*unsigned char tmp = 1;
            unsigned char nibble = 0;
            for(unsigned char j = 0; j < 4; j++){
                nibble = (a[i] & tmp);
                result[k] ^= looksup[nibble];
                tmp <<= 1;
            }*/
            result[k] ^= looksup[a[i] & 0x01];
            result[k] ^= looksup[a[i] & 0x02];
            result[k] ^= looksup[a[i] & 0x04];
            result[k] ^= looksup[a[i] & 0x08];
            a[i] = a[i] >> 4;
            k ++;
            result[k] ^= looksup[a[i] & 0x01];
            result[k] ^= looksup[a[i] & 0x02];
            result[k] ^= looksup[a[i] & 0x04];
            result[k] ^= looksup[a[i] & 0x08];
            k ++;
/*
            tmp = 1;
            k ++;
            for(unsigned char j = 0; j < 4; j++){
                nibble = (a[i] & tmp);
                result[k] ^= looksup[nibble];
                tmp <<= 1;
            }
            k ++;
            */
    }
    reduce(result);
    copyPoly(a, result);
    return a;
}

inline unsigned char * mulRed(unsigned char * a, unsigned char * b, unsigned char * result){
        mul(a, b, result);
        reduce(result);
        return result;
}

inline unsigned char inverseBit(unsigned char a){
    unsigned char res = 0;
    unsigned char tmp = 1;
    for(unsigned char i = 0; i < 8; i ++){
        if ((a & tmp) == 0x01){
            res ^= 0x80;
        }else if((a & tmp) == 0x02){
            res ^= 0x40;
        }else if((a & tmp) == 0x04){
            res ^= 0x20;
        }else if((a & tmp) == 0x08){
            res ^= 0x10;
        }else if((a & tmp) == 0x10){
            res ^= 0x08;
        }else if((a & tmp) == 0x20){
            res ^= 0x04;
        }else if((a & tmp) == 0x40){
            res ^= 0x02;
        }else if((a & tmp) == 0x80){
            res ^= 0x01;
        }
        tmp <<= 1;
    }
    return res;
}

unsigned char * inversBitsInPoly(unsigned char * a, unsigned char sizz = 10){
    unsigned char sz = sizz;
    unsigned char result[10];
    cleanPoly(result);
    unsigned char k = sz - 1;
    for(unsigned char i = 0; i < sz; i ++){
        result[k] = a[i];
        result[k] = inverseBit(result[k]);
        k --;
    }
    copyPoly(a,result);
    return a;
}

inline unsigned char isInfinity(Point * P){
  return isEqualPoly(I_x,P->x) && isEqualPoly(I_y,P->y);
}

inline bool isNeg(Point *p, Point *q)
{
    Point tmp;
    memcpy(tmp.x, q->x, SIZE);
    memcpy(tmp.y, q->y, SIZE);
    unsigned char tmx[SIZE];
    cleanPoly(tmx);
    copyPoly(tmx,tmp.x);

    memcpy(tmp.y, add(tmx, tmp.y), SIZE);
    return (memcmp(tmp.x, p->x, SIZE) == 0 && memcmp(tmp.y, p->y, SIZE) == 0);
}

void addPoints(Point * P, Point * Q, Point * R){
    if(isInfinity(P)){
        copyPoly(R->x, Q->x);
        copyPoly(R->y, Q->y);
        return;
    }else if(isInfinity(Q)){
        copyPoly(R->x, P->x);
        copyPoly(R->y, P->y);
        return;
    }else if(isNeg(P,Q) || isNeg(Q,P)){
        copyPoly(R->x, I_x);
        copyPoly(R->y, I_y);
        return;
    }
    //unsigned char lambda[SIZE];
    unsigned char * lambda;
    unsigned char result[SIZE];
    unsigned char addqx[SIZE];
    unsigned char mul_tmp[SIZE];
    if (isEqualPoly(P->x, Q->x) && isEqualPoly(P->y, Q->y)){ // P == Q
        unsigned char inv_qx[SIZE];
        cleanPoly(inv_qx);
        inverse(p, Q->x, inv_qx); // 1/xQ
        cleanPoly(mul_tmp);
        mulRed(Q->y, inv_qx, mul_tmp); // yQ*(1/xQ)
        copyPoly(addqx,Q->x);
        //copyPoly(lambda, add(addqx, mul_tmp));
        lambda = add(addqx, mul_tmp);
    }else{                                                   // P != Q
        unsigned char tmp_top[SIZE];
        cleanPoly(tmp_top);
        add(tmp_top, P->y);
        add(tmp_top, Q->y); // yP + yQ
        unsigned char tmp_down[SIZE];
        cleanPoly(tmp_down);
        add(tmp_down, P->x);
        add(tmp_down, Q->x); // xP + xQ
        unsigned char inv[SIZE];
        cleanPoly(inv);
        inverse(p, tmp_down, inv); // 1/(xP + xQ)
        cleanPoly(result);
        mulRed(tmp_top, inv, result); // 1/(xP + xQ) * (yP + yQ)
        lambda = result; // lambda = 1/(xP + xQ) * yP + yQ
    }
    unsigned char cpa[SIZE];
    copyPoly(cpa, POLA);
    unsigned char sqlam[SIZE];
    copyPoly(sqlam, lambda);
    //xR = a + λ^2 + λ + xP + xQ
    add(cpa, lambda); // a* = a + λ
    add(cpa, square(sqlam)); // a** = a* + λ^2
    add(cpa, P->x); // a*** = a** + P->x
    add(cpa, Q->x); // a*** = a** + Q->x
    copyPoly(R->x,cpa);
    // yR = (xQ + xR) · λ + xR + yQ
    unsigned char cpq[SIZE];
    unsigned char rsl[SIZE];
    cleanPoly(rsl);
    copyPoly(cpq, Q->x); // xQ
    add(cpq, R->x); // xQ + xR
    mulRed(cpq, lambda, rsl); // (xQ + xR) · λ
    add(rsl,R->x); // xR
    add(rsl,Q->y); // yQ
    copyPoly(R->y,rsl);
}
char testSquare(unsigned char * y){
    unsigned char cpy[SIZE];
    copyPoly(cpy, y);
    unsigned char cpys[SIZE];
    copyPoly(cpys, y);
    square(cpy);
    unsigned char multss[SIZE];
    cleanPoly(multss);
    mulRed(y, cpys, multss);
    if(isEqualPoly(multss,cpy))
        return 1;
    return 0;
}

//y^2 + xy ≡ x^3 + a*x^2 + b mod F(α),
char weierstrass(unsigned char * x, unsigned char * y){
//left
    unsigned char cpy[SIZE];
    copyPoly(cpy, y);
    square(cpy); // y^2
    unsigned char mulres[SIZE];
    cleanPoly(mulres);
    mulRed(x , y, mulres);
    add(mulres, cpy); // y^2 + xy
//right
    unsigned char cpx[SIZE];
    copyPoly(cpx, x);
    square(cpx);
    unsigned char mulcpx[SIZE];
    cleanPoly(mulcpx);
    mulRed(cpx, x, mulcpx); // x^3
    unsigned char mulcpa[SIZE];
    cleanPoly(mulcpa);
    mulRed(cpx, POLA, mulcpa); // a*x^2
    add(mulcpx, mulcpa); // x^3 + a*x^2
    add(mulcpx, POLB); // x^3 + a*x^2 + b
    printA(mulres);
    printA(mulcpx);
    if(isEqualPoly(mulcpx,mulres))
        return 1;
    return 0;
}

unsigned char subsone(unsigned char * numb, unsigned int sz){
    unsigned int i = 0;
    while(1){
        if(i == sz){
            for(unsigned int j=0;j<sz;j++)
                numb[j] = 0;
            return true;
        }
        numb[i] --;
        if (numb[i] == 0x00){
            i ++;
        }else
            break;
    }
    return false;
}

void doubleAndAdd(unsigned char * numb, Point * P, unsigned char sznum){
    Point MP;
    copyPoly(MP.x, P->x);
    copyPoly(MP.y, P->y);
    Point Q;
    cleanPoly(Q.x);
    cleanPoly(Q.y);
    Point tmpA;
    for(unsigned char i=0; i<sznum; i++){
        for(unsigned char j = 1; j !=0 ; j <<= 1){
            if((numb[i] & j) > 0){
                cleanPoly(tmpA.x);
                cleanPoly(tmpA.y);
                addPoints(&MP, &Q, &tmpA);
                //copyPoly(Q.x,tmpA.x);
                //copyPoly(Q.y,tmpA.y);
                Q = tmpA;
                /*Q.x = tmpA.x;
                Q.y = tmpA.y;*/
            }
            copyPoly(tmpA.x, MP.x);
            copyPoly(tmpA.y, MP.y);
            /*Point tmpB;
            copyPoly(tmpB.x, MP.x);
            copyPoly(tmpB.y, MP.y);
            addPoints(&tmpB, &tmpA, &MP);*/
            addPoints(&MP, &tmpA, &MP);
        }
    }
    copyPoly(P->x,Q.x);
    copyPoly(P->y,Q.y);
}

void copyParts(unsigned char * a, unsigned char * b, unsigned char sz){
    for(unsigned char i = 0+sz*10; i < 10+sz*10; i++){
        a[i-sz*10] = b[i];
    }
}

int main(int argc, char *argv[])
{

    /*unsigned char Ka[10] = { 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12}; //, 0xf1 };
    unsigned char Kb[10] = { 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12}; //, 0xf1 };
    unsigned char res1[10];
    cleanPoly(res1);
    mul(Ka,Kb,res1);
    printA(res1);
    unsigned char res2[10];
    cleanPoly(res2);
    mule(Ka,Kb,res2);
    printA(res2);*/

/*
    Point Pa;
    copyPoly(Pa.x,x_P);
    copyPoly(Pa.y,y_P);
    unsigned char Ka[45] = { 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12}; //, 0xf1 };
    // 123456789012345678
    Point tmpA;
    copyPoly(tmpA.x, Pa.x);
    copyPoly(tmpA.y, Pa.y);
    doubleAndAdd(Ka, &tmpA, 45);
    printA(tmpA.x);
    printA(tmpA.y);
*/


    if(argc < 2){
        cout << "None port given" << endl;
        return 1;
    }
// =============================== DH Negotiation ===============================
// ==============================================================================
    int usbtty = atoi(argv[argc-1]);
    char mode[]={'8','N','1',0};
    if(RS232_OpenComport(usbtty, 9600, mode)){
        cout << "Port can't opened" << endl;
        return(0);
    }
    cout << "Port opened" << endl;
// syncing with duino
    int n = 0;
    unsigned char out[34];
    cleanPoly(out);
    RS232_SendByte(usbtty, 0x33);
    while(1){
        sleep(1);
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            cout << hex << (unsigned int) out[0];
            cout << " synced" << endl;
            break;
        }
        sleep(1);
        RS232_SendByte(usbtty, 0x33);
    }
// asking for Ka
    cout << "Asking for Ka" << endl;
    RS232_SendByte(usbtty, 0x67);
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << dec << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    for(unsigned char i = 0; i < 10 ; i++){
        cout << hex << (int) out[i] << " ";
    }
    cout << endl;
// call from DH start
    cout << "Counting our Pa and sendig Duino command to start as well" << endl;
    RS232_SendByte(usbtty, 0x10);
    Point Pa;
    copyPoly(Pa.x,x_P);
    copyPoly(Pa.y,y_P);
    unsigned char Ka[10] = { 0x90, 0x78, 0x56, 0x34, 0x12, 0x90, 0x78, 0x56, 0x34, 0x12}; //, 0xf1 };
    // 123456789012345678
    Point tmpA;
    copyPoly(tmpA.x, Pa.x);
    copyPoly(tmpA.y, Pa.y);
    doubleAndAdd(Ka, &tmpA, 10);
    printA(tmpA.x);
    printA(tmpA.y);
    cleanPoly(out);
    cout << "Waiting fo Duino to finnish" << endl;
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << dec << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    Point tmpB;
    cleanPoly(tmpB.x);
    cleanPoly(tmpB.y);
    copyParts(tmpB.x,out,0);
    copyParts(tmpB.y,out,1);
    cout << "Duino response" << endl;
    printA(tmpB.x);
    printA(tmpB.y);
    cout << "Counting K and asking Duino to do the same" << endl;
    RS232_SendByte(usbtty, 0x11);
    for(unsigned char i = 0; i < 10; i++)
        RS232_SendByte(usbtty, tmpA.x[i]);
    for(unsigned char i = 0; i < 10; i++)
        RS232_SendByte(usbtty, tmpA.y[i]);
    cout << "Wating for ACK of sended tmpA" << endl;
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << dec << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    for(unsigned char i=0; i<20; i++)
        cout << " " << hex << (unsigned int) out[i] << dec;
    cout << endl;
    cout << "Counting K" << endl;
    doubleAndAdd(Ka, &tmpB, 10);
    cout << "K" << endl;
    printA(tmpB.x);
    printA(tmpB.y);

    cout << "Waiting for Duino to finnish" << endl;
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            cout << (unsigned int) n;
            cout << " got response" << endl;
            break;
        }
        sleep(1);
    }
    cout << hex << (unsigned int)out[0] << dec << endl;
    cout << "Asking Duino for Key!!!" << endl;

    RS232_SendByte(usbtty, 0x61);
    while(1){
        n = RS232_PollComport(usbtty, out, 34);
        out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << dec << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    cout << "Key: " << endl;
    for(unsigned char i=0; i<10; i++)
        cout << " " << hex << (unsigned int) out[i] << dec;
    cout << endl;
    for(unsigned char i=10; i<20; i++)
        cout << " " << hex << (unsigned int) out[i] << dec;
    cout << endl;

// =============================== AES TESTS ===============================
// =========================================================================
    cout << "Asking Duino to AES decrypt for me to test it" << endl;
    unsigned char  tmp[SIZE];
    unsigned char  wrd[SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    //unsigned char  key[SIZE] = { 0xc1, 0x74, 0xe7, 0x62, 0x20, 0xa6, 0x18, 0x77, 0x50, 0x63, 0, 0, 0, 0, 0, 0};
    cout << "Open text: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) wrd[i] << dec;
    cout << endl;
    AES_encrypt_128(wrd, tmpB.x, tmp);
    //AES_encrypt_128(wrd, key, tmp);
    cout << "Cypher: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) wrd[i] << dec;
    cout << endl;
    RS232_SendByte(usbtty, 0x22);
    for(unsigned char i = 0; i < 16; i++)
        RS232_SendByte(usbtty, wrd[i]);
    cout << "Waiting fo Duino to finnish" << endl;
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        //out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    cout << "Received: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) out[i] << dec;
    cout << endl;
    cout << "Asking Duino to AES encrypt for me to test it" << endl;
    unsigned char wrc[SIZE] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    //unsigned char  key[SIZE] = { 0xc1, 0x74, 0xe7, 0x62, 0x20, 0xa6, 0x18, 0x77, 0x50, 0x63, 0, 0, 0, 0, 0, 0};
    cout << "Open text: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) wrc[i] << dec;
    cout << endl;
    AES_encrypt_128(wrc, tmpB.x, tmp);
    //AES_encrypt_128(wrd, key, tmp);
    cout << "Cypher: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) wrc[i] << dec;
    cout << endl;
    RS232_SendByte(usbtty, 0x21);
    unsigned char wra[SIZE] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    for(unsigned char i = 0; i < 16; i++)
        RS232_SendByte(usbtty, wra[i]);
    cout << "Waiting fo Duino to finnish" << endl;
    while(1){
        n = RS232_PollComport(usbtty, out, 20);
        //out[n] = '\0';
        if(n!=0){
            if (out[0] == 0x33){
                cout << "Just sync" << endl;
            }else{
                cout << (unsigned int) n;
                cout << " got response" << endl;
                break;
            }
        }
        sleep(1);
    }
    cout << "Received: ";
    for(unsigned char i = 0; i < 16; i++)
        cout << " " << hex << (unsigned int) out[i] << dec;
    cout << endl;

    RS232_SendByte(usbtty, 0x33);
    while(1){
        sleep(1);
        n = RS232_PollComport(usbtty, out, 20);
        out[n] = '\0';
        if(n!=0){
            cout << hex << (unsigned int) out[0];
            cout << " synced" << endl;
            break;
        }
        sleep(1);
        RS232_SendByte(usbtty, 0x33);
    }
    RS232_flushRXTX(usbtty);
    RS232_CloseComport(usbtty);

    return 0;
}
