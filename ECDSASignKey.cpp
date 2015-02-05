#include "ECDSA.h"
#include "ECDSASignKey_err.h"

int ECDSASignKey::toString(char*,size_t)
{
	return 0;
}

int ECDSASignKey::toBinary(byte**,size_t)
{
	return 0;
}

//ECDSASignKey(char*,size_t)
//ECDSASignKey(byte*,size_t)

ECDSASignKey::ECDSASignKey(Big dA,ECn g,Big n)
{
	this->g = g;
	this->n = n;
	this->dA = dA;
}

ECDSASignKey::ECDSASignKey(ECn g,Big n)
{
	this->g = g;
	this->n = n;
	do
	{
		this->dA = rand(n-1);
	}while(this->dA != 1);	
}

ECDSAVerifyKey ECDSASignKey::getVerifyKey()
{	
	ECDSAVerifyKey vk(curve,dA * g);
	return vk;
}

ECDSASignature sign(Serialization* msg,int* err=0)
{
	int _err = SIGNBASE;
	byte* m;
	size_t mLen;
	msg->toBinary(&m,&mLen);
	
	Big z = hash and leftmost...  //Todo
	
	ECDSASignature sig;
	do
	{
		Big k,x1,y1;
		do
		{
			k = rand(n-1);
		}
		while(k!=1);
		ECn kg = k*g;
		kg.getxy(x1,y1);
		sig.r = x1 % n;
		sig.s = inverse(k,n) * (z+r*dA) % n;
	}while(sig.r == 0 || sig.s == 0)
	
	if(err)
		*err = _err;
	return sig;
}