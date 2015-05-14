#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "pairing_3.h"
#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupInit common label";

//Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	PFC pfc(AES_SECURITY);
	miracl *mip=get_mip();
	
	if(argc != 3)
	{
		showUsageAndExit();
	}
	
	const char* common=argv[1];
	const char* label=argv[2];
	
	irand(time(NULL));
	
	ECDSACurve ecdsaCurve = loadECDSACurve(common,mip);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,mip);
	mip->IOBASE = 16;
	
	ECDSASignKey sk(ecdsaCurve);
	ECDSAVerifyKey vk = sk.getVerifyKey();
	ECElgamalDecryptKey dk(ecelgamalCurve);
	ECElgamalEncryptKey ek = dk.getEncrpytKey();
	
	FILE *out;
	char buf[500],filePath[100],fileName[50];
	mkdir(label, 0755);
	cout<<label<<" dir created."<<endl;
	
	mkdir("public", 0755);
	cout<<"public dir created."<<endl;
	
	getFileName(fileName,label,"ek");
	getPath(filePath,"public",fileName);
	cout<<filePath<<" created."<<endl;
	ek.toString(buf,500);
	outputToFile(buf,filePath);
	cout<<buf<<endl;
	
	getFileName(fileName,label,"ekSig");
	getPath(filePath,"public",fileName);
	cout<<filePath<<" created."<<endl;
	BinaryData data = {(byte*)buf,strlen(buf)};
	ECDSASignature sig = sk.sign(data);
	sig.toString(buf,500);
	outputToFile(buf,filePath);
	cout<<buf<<endl;
	
	getPath(filePath,label,"dk");
	cout<<filePath<<" created."<<endl;
	dk.toString(buf,500);
	outputToFile(buf,filePath);
	cout<<buf<<endl;
	
	getPath(filePath,label,"sk");
	cout<<filePath<<" created."<<endl;
	sk.toString(buf,500);
	outputToFile(buf,filePath);
	cout<<buf<<endl;
	
	getFileName(fileName,label,"vk");
	getPath(filePath,"public",fileName);
	cout<<filePath<<" created."<<endl;
	vk.toString(buf,500);
	outputToFile(buf,filePath);
	cout<<buf<<endl;
	
	return 0;
}