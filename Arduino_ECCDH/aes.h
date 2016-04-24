#ifndef aes
#define aes

unsigned char * AES_decrypt_128(unsigned char * wrd, unsigned char * key, unsigned char * tmp);

unsigned char * AES_encrypt_128(unsigned char * wrd, unsigned char * key, unsigned char * tmp);

void rotate(unsigned char *in);

unsigned char rcon(unsigned char in);

unsigned char sbox(unsigned char p_get);

unsigned char sbox_inv(unsigned char p_get);

void translate_matrix(unsigned char * mat);

void printMatrix(unsigned char * wrd, unsigned char x, unsigned char y );

void ShiftRows(unsigned char * wrd,unsigned char * tmp);

void InvShiftRows(unsigned char * wrd,unsigned char * tmp);

void MixFour(unsigned char * wrd);

unsigned char gmul(unsigned char a, unsigned char b);

void InvMixFour(unsigned char * s);

void MixColumns(unsigned char * wrd);

void InvMixColumns(unsigned char * wrd);

void schedule_core(unsigned char *in, unsigned char i);

void expand_key(unsigned char *in);

void addRoundKey(unsigned char * key, unsigned char * wrd);

void InvSubByte(unsigned char * wrd);

void SubByte(unsigned char * wrd);

#endif
