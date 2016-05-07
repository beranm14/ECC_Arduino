#ifndef ecdh
#define ecdh
#define SIZE (20)
#define KA_SIZE (10)

class Point{
    public:
        unsigned char x[SIZE];
        unsigned char y[SIZE];
};

void initialECDH(Point * P, unsigned char * Ka);

unsigned char * copyPoly(unsigned char * a, unsigned char * b);

void cleanPoly(unsigned char * a);

unsigned char * add(unsigned char * a, unsigned char * b, unsigned char from = 0);

bool isNotZero(unsigned char * a);

void shiftLeft(unsigned char * a, unsigned char k = SIZE);

unsigned char isZero(unsigned char * a);

unsigned int getHighBit(unsigned char * a);

inline void addfromto(unsigned char * a, unsigned char * b, unsigned char from, unsigned char to);

unsigned char * mul(unsigned char * a, unsigned char * b, unsigned char * res);

unsigned char * getPoly(unsigned int a, unsigned char * poly);

void divMod(unsigned char * a, unsigned char * b, unsigned char * m, unsigned char * d);

 unsigned char isOne(unsigned char * b);

unsigned char * reduce(unsigned char * c);

 unsigned char isBiggerThanOne(unsigned char * b);

 unsigned char isEqualPoly(unsigned char * a, unsigned char * b);

void inverse(unsigned char * a, unsigned char * b, unsigned char * res);

unsigned char * mulRed(unsigned char * a, unsigned char * b, unsigned char * result);

unsigned char inverseBit(unsigned char a);

unsigned char * inversBitsInPoly(unsigned char * a);

unsigned char * square(unsigned char * a);

 unsigned char isInfinity(Point * P);

 bool isNeg(Point *p, Point *q);

void addPoints(Point * P, Point * Q, Point * R);

//char weierstrass(unsigned char * x, unsigned char * y);

void doubleAndAdd(unsigned char * numb, Point * P, unsigned char sznum = KA_SIZE);

#endif
