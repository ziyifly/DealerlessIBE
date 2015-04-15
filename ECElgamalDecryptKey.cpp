#include "ECElgamal.h"

// Serialization
int ECElgamalDecryptKey::toString(char* buf,size_t sz)
{
	big x;
	int len,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	x = this->x.getbig();
	
	len=cotstr(x,mip->IOBUFF);
	
	if(sz < len)
		return -1;
	for (i=0;i<len;i++) 
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++)='\0';
	
	return ptr-buf;
}
int ECElgamalDecryptKey::toBinary(byte*,size_t)
{
	return 0;
}

// Constructor
//ECElgamalDecryptKey::ECElgamalDecryptKey(char*,size_t){}
ECElgamalDecryptKey::ECElgamalDecryptKey(byte*,size_t){}
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
ECElgamalDecryptKey::ECElgamalDecryptKey(ECElgamalCurve curve,Big x): curve(curve),x(x){}

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

