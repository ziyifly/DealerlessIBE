#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "setupPubKey policyFile label";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 3)
	{
		showUsageAndExit();
	}
	
	const char* policyFileName=argv[1];
	const char* label=argv[2];
	
	char buf[500],filePath[100],fileName[50],*ptr;
	size_t sz;
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	
	Big alpha(0),a(0);
	for(int i=0;i<policy.rowCnt;i++)
	{
		strcpy(fileName,policy.labels[i]);
		strcat(fileName,".alphaRcv");
		getPath(filePath,label,fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		sz = inputFromFile(buf,filePath);
		Big tmp(buf);
		alpha += tmp;
	}
	for(int i=0;i<policy.rowCnt;i++)
	{
		strcpy(fileName,policy.labels[i]);
		strcat(fileName,".aRcv");
		getPath(filePath,label,fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		sz = inputFromFile(buf,filePath);
		Big tmp(buf);
		a += tmp;
	}
	
	G1 g1_alpha = pfc.mult(g1,alpha);
	
	getPath(filePath,label,"g1alpha");
	ofstream outg1alpha(filePath);
	outg1alpha << g1_alpha.g<<endl;
	
	getPath(filePath,label,"alpha");
	ofstream outalpha(filePath);
	outalpha << alpha<<endl;
	
	getPath(filePath,label,"a");
	ofstream outa(filePath);
	outa << a <<endl;
		
	clearPolicy(policy);
	
	cout<<"End"<<endl;
	return 0;
}