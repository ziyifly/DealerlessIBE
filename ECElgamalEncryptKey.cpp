#include "ECElgamal.h"

// Serialization
int ECElgamalEncryptKey::toString(char* buf,size_t sz)
{
	Big X,Y;
	big x,y;
	int nx,ny,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	this->h.get(X,Y);
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
	*(ptr++)='\0';
	
	return ptr-buf;
}
int ECElgamalEncryptKey::toBinary(byte*,size_t)
{
	return 0;
}

// Constructor
ECElgamalEncryptKey::ECElgamalEncryptKey(ECElgamalCurve curve,ECn h): curve(curve), h(h)
{}
/*::ECElgamalEncryptKey(char* buf,size_t)
{
	
}
*/
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
