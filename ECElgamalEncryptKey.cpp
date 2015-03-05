#include "ECElgamal.h"

// Serialization
int ECElgamalEncryptKey::toString(char* buf,size_t sz)
{
	return 0;
}
int ECElgamalEncryptKey::toBinary(byte*,size_t)
{
	return 0;
}

// Constructor
ECElgamalEncryptKey::ECElgamalEncryptKey(ECElgamalCurve curve,ECn h): curve(curve), h(h)
{}

// EncryptKey Method
ECElgamalCiphertext ECElgamalEncryptKey::encrypt(ECn m)
{
	ECElgamalCiphertext ct;
	Big k = rand(curve.n-1);
	ct.c1 = k*curve.g;
	ct.c2 = m;
	ct.c2 += k*h;
	return ct;
}
