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

const char* usage = "tredecrypt number(decimal) recver file outputFile";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 5)
	{
		showUsageAndExit();
	}
	
	const char* timeStr = argv[1];
	const char* recverStr = argv[2];
	const char* cipherDir = argv[3];
	const char* output = argv[4];
	
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
	
	cout<<"Loading user secretKey"<<endl;
	getPath(filePath,recverStr,"sk");
	Big usk= BigFromFile(filePath);
	
	Waters_CPABEPublicKey pk(pfc,g1,g1a,g2,g2a,eggalpha);
	
	cout<<"Loading time-bound key: "<<timeStr<<endl;
	char** attrs = new char*[100];
	size_t attrCnt = getAttrsFromReleaseTime(timeStr,attrs,100);
	getPath(dirPath,"public",timeStr);
	
	cout<<"Attrs = {";
	for(int i=0;i<attrCnt;i++)
	{
		cout<<" "<<attrs[i];
	}
	cout<<"}"<<endl;
	
	getPath(filePath,dirPath,"K");
	G1 K = G1FromFile(filePath);
	
	getPath(filePath,dirPath,"L");
	G1 L = G1FromFile(filePath);
	
	G2* Kx = new G2[attrCnt];
	for(int i=0;i<attrCnt;i++)
	{
		getPath(filePath,dirPath,attrs[i]);
		Kx[i] = G2FromFile(filePath);
	}
	Waters_CPABESecretKey sk(pfc,attrs,K,L,Kx,attrCnt);
	
	cout<<"Loading cipherText"<<endl;
	getPath(filePath,cipherDir,"releaseTime");
	ifstream in(filePath);
	char releaseTimeStr[10];
	in>>releaseTimeStr;
	LSSSPolicy policy = policyFromReleaseTimeStr(releaseTimeStr,MAX_LEN);
	cout<<policy;
	
	getPath(filePath,cipherDir,"R");
	G2 R = G2FromFile(filePath);
	
	getPath(filePath,cipherDir,"CT");
	GT CT = GTFromFile(filePath);
	
	getPath(filePath,cipherDir,"C_prime");
	G2 C_prime = G2FromFile(filePath);
	
	G2* C = new G2[policy.rowCnt];
	G1* D = new G1[policy.rowCnt];
	for(int i=0;i<policy.rowCnt;i++)
	{
		sprintf(fileName,"C_%s",policy.labels[i]);
		getPath(filePath,cipherDir,fileName);
		C[i] = G2FromFile(filePath);
			
		sprintf(fileName,"D_%s",policy.labels[i]);
		getPath(filePath,cipherDir,fileName);
		D[i] = G1FromFile(filePath);
	}
	Waters_CPABECipherText ct(policy,CT,C_prime,C,D);
	
	cout<<"Decrypting"<<endl;
	int err;
	GT M = sk.decrypt(ct,&err) / pfc.power(pfc.pairing(R,g1),usk);
	
	
	if(err == 0)
	{
		cout<<"Decrypt Success"<<endl;
		cout<<"Symmetric Key = "<<M.g<<endl;
		
		Big m;
		ZZn4 _a;
		ZZn2 _x;
		M.g.get(_a);
		_a.get(_x);
		_x.get(m);
		char symKeyBuf[20];
		hashBig(m,symKeyBuf);
		getPath(filePath,cipherDir,"cipherText");
		aesEncryptFile(symKeyBuf,iv,filePath,output,false);
	}
	else
	{
		cout<<"Release Time hasn't reached"<<endl;
	}
	return 0;
}
