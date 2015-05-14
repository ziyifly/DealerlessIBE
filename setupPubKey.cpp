#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>


#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "setupPubKey policyFile";

//Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 2)
	{
		showUsageAndExit();
	}
	
	const char* policyFileName=argv[1];
	
	char buf[500],filePath[100],fileName[50],*ptr;
	size_t sz;
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	G1 g1a;
	G2 g2a;
	GT eggalpha(1);
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"g1a0");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		G1 tmp = G1FromFile(filePath);
		
		g1a = g1a +tmp;
	}
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"g2a0");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		G2 tmp = G2FromFile(filePath);
		
		g2a = g2a + tmp;
	}
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"eggalpha0");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		GT tmp = GTFromFile(filePath);
		
		eggalpha = eggalpha * tmp;
	}
	
	ofstream outg1a("public/g1a");
	ofstream outg2a("public/g2a");
	ofstream outeggalpha("public/eggalpha");
	
	outg1a << g1a.g <<endl;
	outg2a << g2a.g <<endl;
	outeggalpha << eggalpha.g <<endl;
	
	cout<<"End"<<endl;
	return 0;
}