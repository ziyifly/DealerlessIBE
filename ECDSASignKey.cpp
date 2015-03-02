#include "ECDSA.h"

int ECDSASignKey::toString(char* buf,size_t sz)
{
	big dA;
	int len,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	dA = this->dA.getbig();
	
	len=cotstr(dA,mip->IOBUFF);
	
	if(sz < len)
		return -1;
	for (i=0;i<len;i++) 
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++)='\n';
	*(ptr++)='\0';
	
	return ptr-buf;
}

int ECDSASignKey::toBinary(byte*,size_t)
{
	return 0;
}

ECDSASignKey::ECDSASignKey(ECDSACurve curve) : curve(curve)
{
	do
	{
		this->dA = rand(curve.n-1);
	}while(this->dA.isone());
#ifdef DEBUG
	showMsg(dA,"dA=");
#endif
}

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
	}while(this->dA.isone());
#ifdef DEBUG
	showMsg(dA,"dA=");
#endif
}

ECDSAVerifyKey ECDSASignKey::getVerifyKey()
{	
	return ECDSAVerifyKey(curve,dA*curve.g);
}

ECDSASignature ECDSASignKey::sign(BinaryData msg,int* err)
{
	int _err = ECDSASIGNBASE;

	Big z = HashToBig(msg);
	
#ifdef DEBUG
	showMsg(z,"z=");
#endif
	
	ECDSASignature sig;
	do
	{
		Big k,x1;
		do
		{
			k = rand(curve.n-1);
		}
		while(k.isone());
		(k*curve.g).get(x1);
		sig.r = x1 % curve.n;
		sig.s = (inverse(k,curve.n)*(z+sig.r*dA)) % curve.n;
	}while(sig.r.iszero() || sig.s.iszero());
	
	if(err)
	{
		*err = _err;
	}
	
	return sig;
}