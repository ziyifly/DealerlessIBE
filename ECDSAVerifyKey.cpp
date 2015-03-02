#include "ECDSA.h"

int ECDSAVerifyKey::toString(char* buf,size_t sz)
{
	Big X,Y;
	big x,y;
	int nx,ny,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	this->qA.get(X,Y);
	x = X.getbig();
	y = Y.getbig();
	
	nx=cotstr(x,mip->IOBUFF);
	
	*(ptr++)='(';
	if(sz < (ptr+nx)-buf+2)
		return -1;
	for (i=0;i<nx;i++) 
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++)=',';
	
	ny=cotstr(y,mip->IOBUFF);
	if(sz < (ptr+ny)-buf+3)
		return -1;
	for (i=0;i<ny;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++)=')';
	*(ptr++)='\n';
	*(ptr++)='\0';
	
	return ptr-buf;
}

int ECDSAVerifyKey::toBinary(byte*,size_t)
{
	return 0;
}

ECDSAVerifyKey::ECDSAVerifyKey(ECDSACurve curve,ECn qA): curve(curve), qA(qA)
{
}

bool ECDSAVerifyKey::verify(BinaryData msg,ECDSASignature sig)
{
	Big z = HashToBig(msg);
	
#ifdef DEBUG
	showMsg(z,"z=");
#endif

	Big w = inverse(sig.s,curve.n);
	Big u1 = z*w % curve.n;
	Big u2 = sig.r*w % curve.n;
	Big x;
	mul(u1,curve.g,u2,qA).get(x);
	
#ifdef DEBUG
	showMsg(x,"x=");
#endif
	
	return (sig.r == x%curve.n);
}