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

const int MAX_LEN = 6;

const char* usage = "treencrypt number(decimal) file outputFile";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 4)
	{
		showUsageAndExit();
	}
	
	const char* releaseTimeStr = argv[1];
	const char* encryptFile = argv[2];
	const char* outputDir = argv[3];
	
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
	GT eggalpha = GTFromFile(filePath);
	
	Waters_CPABEPublicKey pk(pfc,g1,g1a,g2,g2a,eggalpha);
	
	mkdir(outputDir, 0755);
	
	Big m;
	pfc.random(m);
	GT M = pfc.power(pfc.pairing(g2,g1),m);
	ZZn4 _a;
	ZZn2 _x;
	M.g.get(_a);
	_a.get(_x);
	_x.get(m);
	char symKeyBuf[20];
	hashBig(m,symKeyBuf);
	getPath(filePath,outputDir,"cipherText");
	aesEncryptFile(symKeyBuf,iv,encryptFile,filePath,true);
	cout<<"Symmetric Key = "<<M.g<<endl;
	
	LSSSPolicy policy = policyFromReleaseTimeStr(releaseTimeStr,MAX_LEN); 
	cout<<policy<<endl;
	
	Waters_CPABECipherText ct = pk.encrypt(M,policy);
	
	ofstream out;
	
	getPath(filePath,outputDir,"releaseTime");
	out.open(filePath);
	out<<releaseTimeStr<<endl;
	out.close();
	
	getPath(filePath,outputDir,"CT");
	out.open(filePath);
	out<<ct.CT.g<<endl;
	out.close();
	
	getPath(filePath,outputDir,"C_prime");
	out.open(filePath);
	out<<ct.C_prime.g<<endl;
	out.close();
	
	for(int i=0;i<ct.policy.rowCnt;i++)
	{
		sprintf(fileName,"C_%s",ct.policy.labels[i]);
		getPath(filePath,outputDir,fileName);
		out.open(filePath);
		out<<ct.C[i].g<<endl;
		out.close();
		
		sprintf(fileName,"D_%s",ct.policy.labels[i]);
		getPath(filePath,outputDir,fileName);
		out.open(filePath);
		out<<ct.D[i].g<<endl;
		out.close();
	}
	
	clearPolicy(policy);
	return 0;
}
