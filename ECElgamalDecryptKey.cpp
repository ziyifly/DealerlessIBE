#include "ECElgamal.h"

int ECElgamalDecryptKey::toString(char* buf,size_t sz)
{
}
int ECElgamalDecryptKey::toBinary(byte*,size_t)
{
}

ECElgamalDecryptKey::ECElgamalDecryptKey()
//...

ECn ECElgamalDecryptKey::decrypt(ECElgamalCiphertext ct)
{
	return ct.c2-x*ct.c1;
}
ECElgamalEncryptKey ECElgamalDecryptKey::getEncrpytKey()
{
	return ECElgamalEncryptKey(curve,x*curve.g);
}

