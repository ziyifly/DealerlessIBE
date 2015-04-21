#include "ECElgamal.h"

// Serialization
int ECElgamalCiphertext::toString(char* buf,size_t sz)
{
	Big x1,y1;
	Big x2,y2;
	int n,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	c1.get(x1,y1);
	c2.get(x2,y2);
	
	*(ptr++) = '(';
	n = cotstr(x1.getbig(),mip->IOBUFF);
	if(ptr+n-buf+1 > sz)
		return -1;
	for (i=0;i<n;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ',';
	n = cotstr(y1.getbig(),mip->IOBUFF);
	if(ptr+n-buf+3 > sz)
		return -1;
	for (i=0;i<n;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ')';
	
	*(ptr++) = '\n';
	*(ptr++) = '(';
	n = cotstr(x2.getbig(),mip->IOBUFF);
	if(ptr+n-buf+1 > sz)
		return -1;
	for (i=0;i<n;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ',';
	n = cotstr(y2.getbig(),mip->IOBUFF);
	if(ptr+n-buf+2 > sz)
		return -1;
	for (i=0;i<n;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ')';
	*(ptr++) = '\0';
	
	return ptr-buf;
}
int ECElgamalCiphertext::toBinary(byte* buf,size_t sz)
{
	return 0;
}

// Constructor
//ECElgamalCiphertext::ECElgamalCiphertext(char*,size_t){}
ECElgamalCiphertext::ECElgamalCiphertext(){}
ECElgamalCiphertext::ECElgamalCiphertext(ECn c1,ECn c2):c1(c1),c2(c2){}