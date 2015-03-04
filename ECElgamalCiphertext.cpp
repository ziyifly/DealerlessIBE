#include "ECElgamal.h"

// Serialization
int ECElgamalCiphertext::toString(char* buf,size_t sz)
{
	Big x1,y1;
	Big x2,y2;
	int n,i;
	char* ptr = buf;
	miracl *mip=get_mip();
	
	c1.getxy(x1,y1);
	c2.getxy(x2,y2);
	
	*(ptr++) = '(';
	n = cotstr(x1,mip->IOBUFF);
	if(ptr+n-buf+1 > sz)
		return -1;
	for (i=0;i<nr;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ',';
	n = cotstr(x1,mip->IOBUFF);
	if(ptr+n-buf+3 > sz)
		return -1;
	for (i=0;i<nr;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ')';
	
	*(ptr++) = '\n';
	*(ptr++) = '(';
	if(ptr+n-buf+1 > sz)
		return -1;
	for (i=0;i<nr;i++)
		*(ptr++)=mip->IOBUFF[i];
	*(ptr++) = ',';
	if(ptr+n-buf+2 > sz)
		return -1;
	for (i=0;i<nr;i++)
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
ECElgamalCiphertext::ECElgamalCiphertext(char*,size_t)
{}
ECElgamalCiphertext::ECElgamalCiphertext(){}