#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

const char* usage = "treencrypt number(decimal) file";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 3)
	{
		showUsageAndExit();
	}
	
	const char* releaseTimeStr = argv[1];
	const char* encryptFile = argv[2];
	
	char filePath[100],fileName[100];
	char dirPath[100];
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	
	cout<<"Loading g2..."<<endl;
	getPath(filePath,"public","g2");
	G2 g2 = G2FromFile(filePath);
	
	cout<<"Loading pubKey"<<endl;
	getPath(filePath,"public","g1a");
	G1 g1a = G1FromFile(filePath);
	
	getPath(filePath,"public","g2a");
	G2 g2a = G2FromFile(filePath);
	
	getPath(filePath,"public","eggalpha");
	GT eggalpha = GTFromFile(eggalpha);
	
	Waters_CPABEPublicKey pk(pfc,g1,g1a,g2,g2a,eggalpha);
	
	char buf[1000]; // Message
	
	Big m;
	pfc.random(m);
	GT M = pfc.power(pfc.pairing(g2,g1),m);
	ZZn2 _x;
	M.a.get(_x);
	_x.get(m);
	char symKeyBuf[20];
	hashBig(x,symKeyBuf);
	aesEncrypt(symKeyBuf,buf,sz,iv);
	
	int releaseTime = atoi(releaseTimeStr);
	LSSSPolicy policy = policyFromReleaseTime(releaseTime); //** TODO
	
	Waters_CPABECipherText ct = pk.encrypt(M,policy);
	
	//output
	
	return 0;
}
