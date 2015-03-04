#include "ECElgamal.h"

// Serialization
int ECElgamalDecryptKey::toString(char* buf,size_t sz)
{
	return 0;
}
int ECElgamalDecryptKey::toBinary(byte*,size_t)
{
	return 0;
}

// Constructor
ECElgamalDecryptKey::ECElgamalDecryptKey(){}

ECElgamalDecryptKey(char*,size_t){}
ECElgamalDecryptKey(byte*,size_t){}
ECElgamalDecryptKey(Big x,ECn g,Big n): curve({g,n}),x(x){}
ECElgamalDecryptKey(ECn g,Big n)
{
	curve = {g,n};
	do
	{
		x = rand(curve.n-1);
	}while(x.isone());
}
ECElgamalDecryptKey(ECElgamalCurve curve): curve(curve)
{
	do
	{
		x = rand(curve.n-1);
	}while(x.isone());
}

// DecryptKey Method
ECn ECElgamalDecryptKey::decrypt(ECElgamalCiphertext ct)
{
	return ct.c2-x*ct.c1;
}
ECElgamalEncryptKey ECElgamalDecryptKey::getEncrpytKey()
{
	return ECElgamalEncryptKey(curve,x*curve.g);
}

