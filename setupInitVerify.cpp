#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupInitVerify common label";

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
	const char* label=argv[2];
	
	char buf[500],fileName[50];
	size_t sz;
	
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	getPath(fileName,label,"sig");
	sz = inputFromFile(buf,fileName);
	cout<<fileName<<" read."<<endl;
	cout<<buf;
	ECDSASignature sig(buf,sz);
	
	getPath(fileName,label,"vk");
	sz = inputFromFile(buf,fileName);
	cout<<fileName<<" read."<<endl;
	cout<<buf;
	ECn qA = ECnFromStr(buf);
	ECDSAVerifyKey vk(ecdsaCurve,qA);
	
	getPath(fileName,label,"ek");
	sz = inputFromFile(buf,fileName);
	cout<<fileName<<" read."<<endl;
	ECn h = ECnFromStr(buf);
	ECElgamalEncryptKey ek(ecelgamalCurve,h);
	ek.toString(buf,500);
	
	BinaryData data = {(byte*)buf,strlen(buf)};
	if(vk.verify(data,sig))
	{
		cout<<"Verify Success!!"<<endl;
	}
	else
	{
		cout<<"Verify Fail!!"<<endl;
	}
	
	return 0;
}