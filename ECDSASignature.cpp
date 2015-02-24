#include "ECDSA.h"

int ECDSASignature::toString(char* buf,size_t sz)
{
	big r,s;
	int nr,ns,i;
	miracl *mip=get_mip();
	
	r = this->r.getbig();
	s = this->s.getbig();
	
	nr=cotstr(r,mip->IOBUFF);
	
	if(sz < nr)
		return -1;
	for (i=0;i<nr;i++) 
		buf[i]=mip->IOBUFF[i];
	buf[nr]='\n';
	
	ns=cotstr(s,mip->IOBUFF);
	if(sz<nr+ns+1)
		return -1;
	
	for (i=0;i<ns;i++)
		buf[nr+1+i]=mip->IOBUFF[i];
	buf[nr+ns+1]=0;
	
	return nr+ns;
}
int ECDSASignature::toBinary(byte*,size_t)
{
	return 0;
}
ECDSASignature::ECDSASignature(char* buf,size_t sz)
{
	char* ptr = strtok(buf,"\n");
	r = Big(ptr);
	ptr = strtok(NULL,"\n");
	s = Big(ptr);
}
ECDSASignature::ECDSASignature(){}