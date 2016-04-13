#define SIZE (20)

unsigned char POLA[SIZE] = { 0x5F, 0x38, 0x4C, 0x7F, 0x6D, 0xF6, 0xA8, 0x38, 0x2E, 0x4A };
unsigned char POLB[SIZE] = { 0xA7, 0x68, 0x3D, 0xC0, 0xEC, 0x6B, 0x1C, 0xB3, 0x0B, 0x2C };
unsigned char p[SIZE] = { 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };

unsigned char  P_x[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  P_y[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  Q_x[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  Q_y[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  R_x[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char  R_y[SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


class Point{
    public:
        unsigned char x[SIZE];
        unsigned char y[SIZE];
};

unsigned char * add(unsigned char * a, unsigned char * b, unsigned char from = 0){
    for(unsigned char i = from; i < SIZE; i++){
        a[i] = a[i] ^ b[i];
    }
    return a;
}


bool isNotZero(unsigned char * a){
    for(unsigned char i = 0; i < SIZE; i++){
        if (a[i] != 0)
            return true;
    }
    return false;
}

void shiftLeft(unsigned char * a, unsigned char k = SIZE){
    unsigned char tmp = 0;
    for (unsigned char j = 0 ; j < k; j++){
        unsigned char tmr = a[j] & 0x80;
        a[j] <<= 1;
        if (tmp)
            a[j] |= 1;
        tmp = tmr;
    }
}

unsigned char isZero(unsigned char * a){
    for(unsigned char i = 0; i < SIZE; i++){
        if(a[i] != 0){
            return false;
        }
    }
    return true;
}

unsigned int getHighBit(unsigned char * a){
   if(isZero(a))
        return 0;
    unsigned int bits_size = SIZE * 8;
    unsigned int l = 0;
    for (int i = SIZE - 1; i != -1; i --){
        unsigned char tmp = 0x80;
        while ((a[i] & tmp) == 0 && tmp != 0){
            tmp >>= 1;
            l ++;
        }
        if(tmp != 0)
            break;
    }
    l = bits_size - l;
    return l;
}

unsigned char * copyPoly(unsigned char * a, unsigned char * b){
    for(unsigned char i = 0; i < SIZE; i++){
        a[i] = b[i];
    }
    return a;
}
void cleanPoly(unsigned char * a){
    for(unsigned char i = 0; i < SIZE; i++){
        a[i] = 0;
    }
}

unsigned char * mul(unsigned char * a, unsigned char * b, unsigned char * res){
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
}

unsigned char * getPoly(unsigned int a, unsigned char * poly){
    unsigned int where = a / 8;
    unsigned int which = a % 8;
    poly[where] = 1;
    while (which){
        poly[where] <<= 1;
        which --;
    }
    return poly;
}

void divMod(unsigned char * a, unsigned char * b, unsigned char * m, unsigned char * d){
    unsigned char tmp_a[SIZE];
    copyPoly(tmp_a,a);
    if (isZero(a)|| isZero(b) || getHighBit(a) < getHighBit(b)){
        copyPoly(m, a);
        cleanPoly(d);
        return;
    }
    unsigned int dr;
    while( 1 ){
        unsigned char tmpl[SIZE];
        cleanPoly(tmpl);
        unsigned char tmpp[SIZE];
        cleanPoly(tmpp);
        unsigned char tmpres[SIZE];
        cleanPoly(tmpres);
        //
        dr = (getHighBit(a) - getHighBit(b));
        //cout << (unsigned int) dr << endl;
        //return;
        add(d,getPoly(dr,tmpp));
        copyPoly(m, add(a, mul(getPoly(dr,tmpp),b,tmpres))); //*m = a ^ mulPoly(getPoly(dr),b);
        if(getHighBit(m) < getHighBit(b))
            break;
        else{
            copyPoly(a, m);
        }
    }
    copyPoly(a,tmp_a);
}

unsigned char isOne(unsigned char * b){
    for(unsigned char i = 1; i < SIZE; i++){
        if(b[i] != 0)
            return 0;
    }
    if(b[0] == 0x01)
        return 1;
    return 0;
}

unsigned char * reduce(unsigned char * b){
    unsigned char m[SIZE], q[SIZE];
    divMod(b, p, m, q);
    copyPoly(b,m);
    return b;
}

unsigned char isBiggerThanOne(unsigned char * b){
    for(unsigned char i = 1; i < SIZE; i++){
        if(b[i] != 0)
            return 1;
    }
    if(b[0] > 1)
        return 1;
    return 0;
}

unsigned char isEqualPoly(unsigned char * a, unsigned char * b){
    for(unsigned char i = 0; i < SIZE; i++){
        if(a[i] != b[i])
            return false;
    }
    return true;
}


void inverse(unsigned char * a, unsigned char * b, unsigned char * res){
    unsigned char tmp_a[SIZE];
    copyPoly(tmp_a, a);
    unsigned char tmp_b[SIZE];
    copyPoly(tmp_b, b);

  unsigned char b0[SIZE], t[SIZE], q[SIZE];
    copyPoly(b0,b);
    cleanPoly(q);
    unsigned char x0[SIZE], x1[SIZE];
    cleanPoly(x0);
    x0[0] = 0x01;
    cleanPoly(x1);

  if (isOne(b)) return;
  while (isBiggerThanOne(a)) {
        unsigned char tmp[SIZE];
        cleanPoly(tmp);
        cleanPoly(q);
        divMod(a, b, tmp, q);// q = a / b;
        copyPoly(t, b);// t = b;
        copyPoly(b, tmp);// b = tmp;
        copyPoly(a, t); // a = t;
    copyPoly(t, x0); //t = x0;
        unsigned char restmp[SIZE];
        cleanPoly(restmp);
        copyPoly(x0, add(x1, mul(q, x0, restmp))); // x0 = x1 + q * x0;
    copyPoly(x1, t); //x1 = t;
    }
  copyPoly(res, x1);
  copyPoly(a,tmp_a);
    copyPoly(b,tmp_b);
}



unsigned char * mulRed(unsigned char * a, unsigned char * b, unsigned char * result){
        mul(a, b, result);
        reduce(result);
        return result;
}
unsigned char inverseBit(unsigned char a){
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

unsigned char * inversBitsInPoly(unsigned char * a){
    unsigned char sz = SIZE / 2;
    unsigned char result[SIZE / 2];
    cleanPoly(result);
    unsigned char k = sz - 1;
    for(unsigned char i = 0; i < sz; i ++){
        result[k] = a[i];
        //result[k] = inverseBit(result[k]);
        k --;
    }
    copyPoly(a,result);
    return a;
}

unsigned char * square(unsigned char * a){
    unsigned char looksup[9] = {0x00, 0x01, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00, 0x40};
    unsigned char result[SIZE];
    unsigned char k = 0;
    cleanPoly(result);
    for(unsigned char i = 0; i < SIZE; i++){
            unsigned char tmp = 1;
            unsigned char nibble = 0;
            for(unsigned char j = 0; j < 4; j++){
                nibble = (a[i] & tmp);
                result[k] ^= looksup[nibble];
                tmp <<= 1;
            }
            a[i] = a[i] >> 4;
            tmp = 1;
            k ++;
            for(unsigned char j = 0; j < 4; j++){
                nibble = (a[i] & tmp);
                result[k] ^= looksup[nibble];
                tmp <<= 1;
            }
            k ++;
    }
    reduce(result);
    copyPoly(a, result);
    return a;
}

void addPoints(Point * P, Point * Q, Point * R){
    unsigned char lambda[SIZE];
    if (isEqualPoly(P->x, Q->x) && isEqualPoly(P->y, Q->y)){ // P == Q
        unsigned char inv_qx[SIZE];
        cleanPoly(inv_qx);
        inverse(p, Q->x, inv_qx); // 1/xQ
        unsigned char mul_tmp[SIZE];
        cleanPoly(mul_tmp);
        mulRed(Q->y, inv_qx, mul_tmp); // yQ*(1/xQ)
        unsigned char addqx[SIZE];
        copyPoly(addqx,Q->x);
        copyPoly(lambda, add(addqx, mul_tmp));
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
        unsigned char result[SIZE];
        cleanPoly(result);
        mulRed(tmp_top, inv, result); // 1/(xP + xQ) * (yP + yQ)
        copyPoly(lambda, result); // lambda = 1/(xP + xQ) * yP + yQ
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

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
}

void printA(unsigned char * a){
    for(unsigned char i = 0; i < 10; i++){
        Serial.write(a[i]);
    }
}

void loop() {

  if (Serial.available() > 0) {
      byte keyk = Serial.read();
      if(keyk == 0x10){
        byte cn = 0;
        while (cn < 10){ // P_x
          if (!Serial.available())
            continue;
          P_x[cn] = Serial.read();
          cn ++;           
        }
        for(byte i = 0; i < 10; i++)
          Serial.write(P_x[i]);
      }else if(keyk == 0x11){ // P_y
        byte cn = 0;
        while (cn < 10){
          if (!Serial.available())
            continue;
          P_y[cn] = Serial.read();
          cn ++;           
        }
        for(byte i = 0; i < 10; i++)
          Serial.write(P_y[i]);
      }else if(keyk == 0x20){ // Q_x
        byte cn = 0;
        while (cn < 10){
          if (!Serial.available())
            continue;
          Q_x[cn] = Serial.read();
          cn ++;           
        }
        for(byte i = 0; i < 10; i++)
          Serial.write(Q_x[i]);
      }else if(keyk == 0x21){ // Q_y
        byte cn = 0;
        while (cn < 10){
          if (!Serial.available())
            continue;
          Q_y[cn] = Serial.read();
          cn ++;           
        }
        for(byte i = 0; i < 10; i++)
          Serial.write(Q_y[i]);
      }else if(keyk == 0x01){
        // adding
        Point P;
        Point Q;
        copyPoly(P.x,P_x);
        copyPoly(P.y,P_y);
        copyPoly(Q.x,Q_x);
        copyPoly(Q.y,Q_y);        
        Point R;
        cleanPoly(R.x);
        cleanPoly(R.y);
        addPoints(&P, &Q, &R);
        for(byte i = 0; i < 10; i++)
          Serial.write(R.x[i]);
        for(byte i = 0; i < 10; i++)
          Serial.write(R.y[i]);
      }
   }
}
