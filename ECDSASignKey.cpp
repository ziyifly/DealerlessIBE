#include "ECDSA.h"

int ECDSASignKey::toString(char*,size_t)
{
	return 0;
}

int ECDSASignKey::toBinary(byte*,size_t)
{
	return 0;
}

//ECDSASignKey(char*,size_t)
//ECDSASignKey(byte*,size_t)

ECDSASignKey::ECDSASignKey(Big dA,ECn g,Big n)
{
	this->curve.g = g;
	this->curve.n = n;
	this->dA = dA;
}

ECDSASignKey::ECDSASignKey(ECn g,Big n)
{
	this->curve.g = g;
	this->curve.n = n;
	do
	{
		this->dA = rand(n-1);
	}while(this->dA != 1);	
}

ECDSAVerifyKey ECDSASignKey::getVerifyKey()
{	
	return ECDSAVerifyKey(curve,dA * curve.g);
}

ECDSASignature ECDSASignKey::sign(BinaryData msg,int* err=0)
{
	int _err = ECDSASIGNBASE;

	Big z = HashToBig(msg);
	
	ECDSASignature sig;
	do
	{
		Big k,x1,y1;
		do
		{
			k = rand(curve.n-1);
		}
		while(k!=1);
		ECn kg = k*curve.g;
		kg.getxy(x1,y1);
		sig.r = x1 % curve.n;
		sig.s = inverse(k,curve.n) * (z+sig.r*dA) % curve.n;
	}while(sig.r == 0 || sig.s == 0);
	
	if(err)
	{
		*err = _err;
	}
	
	return sig;
}