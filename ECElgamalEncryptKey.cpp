#include "ECElgamal.h"

int ECElgamalEncryptKey::toString(char* buf,size_t sz)
{
}
int ECElgamalEncryptKey::toBinary(byte*,size_t)
{
}
ECElgamalEncryptKey::ECElgamalEncryptKey()
{
}
//...

ECElgamalCiphertext ECElgamalEncryptKey::encrypt(ECn m)
{
	ECElgamalCiphertext ct;
	Big k = rand(curve.n-1);
	ct.c1 = k*curve.g;
	ct.c2 = m + k*h;
	return ct;
}
