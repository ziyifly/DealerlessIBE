#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
using namespace std;

Miracl precision = 20;

int main(int argc,char** argv)
{
	const char *common = argv[1];
	const char *dkFile = argv[2];
	const char *cipherFile = argv[3];
	const char *outputFile = argv[4];
	
	size_t sz;
	char buf[1000];
	miracl *mip=get_mip();
	
	cout<<"Loading curves..."<<endl;
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	mip->IOBASE = 16;
	
	cout<<"Loading dk..."<<endl;
	sz = inputFromFile(buf,dkFile);
	Big x(buf),y;
	ECElgamalDecryptKey dk(ecelgamalCurve,x);
	
	cout<<"Loading ct..."<<endl;
	sz = inputFromFile(buf,cipherFile);
	ECn c1 = ECnFromStr(buf);
	ECn c2 = ECnFromStr(NULL);
	ECElgamalCiphertext ct(c1,c2);
	cout<<"C1 = "<<c1<<"\nC2 = "<<c2<<endl;
	
	cout<<"Decrypting share..."<<endl;
	ECn plaintext = dk.decrypt(ct);
	cout<<"Plaintext= "<<plaintext<<endl;
	
	ofstream output(outputFile);
	plaintext.getxy(x,y);
	output << x << endl;
	
	return 0;
}