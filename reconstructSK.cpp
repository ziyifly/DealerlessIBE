#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "reconstructSK policy number(decimal) label+";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

void reconstructAndOutputG1(const char dirPath[],const char* servs[],int servCnt,const char varName[],LSSS<G1> lsssg1,Share<G1> sharesg1[])
{
	char filePath[100],fileName[100];
	
	G1 g;
	for(int i=0;i<servCnt;i++)
	{
		getFileName(fileName,servs[i],varName);
		getPath(filePath,dirPath,fileName);
		sharesg1[i].share = G1FromFile(filePath);
	}
	lsssg1.reconstructSecret(sharesg1,servCnt,&g);
	getPath(filePath,dirPath,varName);
	ofstream out(filePath);
	out<<g.g<<endl;
}

void reconstructAndOutputG2(const char dirPath[],const char* servs[],int servCnt,const char varName[],LSSS<G2> lsssg2,Share<G2> sharesg2[])
{
	char filePath[100],fileName[100];
	
	G2 g;
	for(int i=0;i<servCnt;i++)
	{
		getFileName(fileName,servs[i],varName);
		getPath(filePath,dirPath,fileName);
		sharesg2[i].share = G2FromFile(filePath);
	}
	lsssg2.reconstructSecret(sharesg2,servCnt,&g);
	getPath(filePath,dirPath,varName);
	ofstream out(filePath);
	out<<g.g<<endl;
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	setLSSSPFC(&pfc);
	
	if(argc < 4)
	{
		showUsageAndExit();
	}
	
	const char* policyFileName=argv[1];
	const char* releaseTimeStr = argv[2];
	const char** labels = (const char**)(argv+3);
	size_t servCnt = argc - 3;
	
	char filePath[100],fileName[100];
	char dirPath[100];
	
	ofstream out;
	
	getPath(dirPath,"public",releaseTimeStr);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	LSSS<G1> lsssg1(policy);
	LSSS<G2> lsssg2(policy);
	
	Share<G1>* sharesg1 = new Share<G1>[servCnt];
	Share<G2>* sharesg2 = new Share<G2>[servCnt];
	for(int i=0;i<servCnt;i++)
	{
		sharesg1[i].label = (char*)labels[i];
		sharesg2[i].label = (char*)labels[i];
	}
	
	reconstructAndOutputG1(dirPath,labels,servCnt,"K",lsssg1,sharesg1);
	reconstructAndOutputG1(dirPath,labels,servCnt,"L",lsssg1,sharesg1);
	
	char** attrs = new char*[100];
	size_t attrCnt = getAttrsFromReleaseTime(releaseTimeStr,attrs,100);
	
	for(int i=0;i<attrCnt;i++)
	{
		reconstructAndOutputG2(dirPath,labels,servCnt,attrs[i],lsssg2,sharesg2);
	}
	
	return 0;
}
	