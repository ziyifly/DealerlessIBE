#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupPubKey common policyFile";

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
	else
	{
		cout<<"Initial.."<<endl;
	}
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	
	char buf[500],filePath[100],fileName[50],*ptr;
	size_t sz;
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	ECn pk;
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"token");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		sz = inputFromFile(buf,filePath);
		ECn tmp = ECnFromStr(buf);
		pk += tmp;
	}
	
	ostringstream oss;
	oss<<pk;
	outputToFile(oss.str().c_str(),"public/pubKey");
	
	clearPolicy(policy);
	
	cout<<"End"<<endl;
	return 0;
}