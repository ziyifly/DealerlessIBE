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
	
	char buf[500],filePath[100],fileName[50];
	size_t sz;
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	getFileName(fileName,label,"ekSig");
	getPath(filePath,"public",fileName);
	cout<<"Loading "<<filePath<<" ..."<<endl;
	sz = inputFromFile(buf,filePath);
	cout<<buf;
	ECDSASignature sig(buf,sz);
	
	getFileName(fileName,label,"vk");
	getPath(filePath,"public",fileName);
	cout<<"Loading "<<filePath<<" ..."<<endl;
	sz = inputFromFile(buf,filePath);
	cout<<buf;
	ECn qA = ECnFromStr(buf);
	ECDSAVerifyKey vk(ecdsaCurve,qA);
	
	getFileName(fileName,label,"ek");
	getPath(filePath,"public",fileName);
	sz = inputFromFile(buf,filePath);
	cout<<"Loading "<<filePath<<" ..."<<endl;
	cout<<sz<<" bytes:"<<buf<<endl;
	ECn h = ECnFromStr(buf);
	ECElgamalEncryptKey ek(ecelgamalCurve,h);
	sz = ek.toString(buf,500);
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