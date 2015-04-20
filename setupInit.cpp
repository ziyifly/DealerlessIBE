#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupInit common label";

Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	if(argc != 3)
	{
		showUsageAndExit();
	}
	
	const char* common=argv[1];
	//const char* policyFile=argv[2];
	const char* label=argv[2];
	
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	//LSSSPolicy policy = loadPolicy(policyFile);
	
	ECDSASignKey sk(ecdsaCurve);
	ECDSAVerifyKey vk = sk.getVerifyKey();
	ECElgamalDecryptKey dk(ecelgamalCurve);
	ECElgamalEncryptKey ek = dk.getEncrpytKey();
	
	FILE *out;
	char buf[500],fileName[50];
	mkdir(label, 0755);
	cout<<label<<" dir created."<<endl;
	
	getPath(fileName,label,"ek");
	cout<<fileName<<" created."<<endl;
	ek.toString(buf,500);
	outputToFile(buf,fileName);
	cout<<buf<<endl;
	
	getPath(fileName,label,"sig");
	cout<<fileName<<" created."<<endl;
	BinaryData data = {(byte*)buf,strlen(buf)};
	ECDSASignature sig = sk.sign(data);
	sig.toString(buf,500);
	outputToFile(buf,fileName);
	cout<<buf<<endl;
	
	getPath(fileName,label,"dk");
	cout<<fileName<<" created."<<endl;
	dk.toString(buf,500);
	outputToFile(buf,fileName);
	cout<<buf<<endl;
	
	getPath(fileName,label,"sk");
	cout<<fileName<<" created."<<endl;
	sk.toString(buf,500);
	outputToFile(buf,fileName);
	cout<<buf<<endl;
	
	getPath(fileName,label,"vk");
	cout<<fileName<<" created."<<endl;
	vk.toString(buf,500);
	outputToFile(buf,fileName);
	cout<<buf<<endl;
	
	//clearPolicy(policy);
	
	return 0;
}