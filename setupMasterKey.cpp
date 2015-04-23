#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupPubKey common policyFile label";

Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	if(argc != 4)
	{
		showUsageAndExit();
	}
	else
	{
		cout<<"Initial.."<<endl;
	}
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	const char* label=argv[3];
	
	char buf[500],filePath[100],fileName[50],*ptr;
	size_t sz;
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	Big mk(0);
	for(int i=0;i<policy.rowCnt;i++)
	{
		strcpy(fileName,policy.labels[i]);
		strcat(fileName,".shareRcv");
		getPath(filePath,label,fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		sz = inputFromFile(buf,filePath);
		Big tmp(buf);
		mk += tmp;
	}
	
	getPath(filePath,label,"mk");
	ostringstream oss;
	oss<<mk;
	outputToFile(oss.str().c_str(),filePath);
	
	clearPolicy(policy);
	
	cout<<"End"<<endl;
	return 0;
}