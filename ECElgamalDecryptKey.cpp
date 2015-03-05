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
ECElgamalDecryptKey::ECElgamalDecryptKey(char*,size_t){}
ECElgamalDecryptKey::ECElgamalDecryptKey(byte*,size_t){}
ECElgamalDecryptKey::ECElgamalDecryptKey(Big x,ECn g,Big n): curve(),x(x)
{
	curve.g = g;
	curve.n = n;
}
ECElgamalDecryptKey::ECElgamalDecryptKey(ECn g,Big n)
{
	curve.g = g;
	curve.n = n;
	do
	{
		x = rand(curve.n-1);
	}while(x.isone());
}
ECElgamalDecryptKey::ECElgamalDecryptKey(ECElgamalCurve curve): curve(curve)
{
	do
	{
		x = rand(curve.n-1);
	}while(x.isone());
}

// DecryptKey Method
ECn ECElgamalDecryptKey::decrypt(ECElgamalCiphertext ct)
{
	ECn tmp = ct.c2;
	tmp += (-x)*ct.c1;
	return tmp;
}
ECElgamalEncryptKey ECElgamalDecryptKey::getEncrpytKey()
{
	return ECElgamalEncryptKey(curve,x*curve.g);
}

