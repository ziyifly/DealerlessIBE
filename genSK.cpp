#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "genSK label number(decimal)";

PFC pfc(AES_SECURITY);  // initialise PFC

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	miracl* mip=get_mip();
	
	if(argc != 3)
	{
		showUsageAndExit();
	}
	
	const char* label = argv[1];
	const char* releaseTimeStr = argv[2];
	
	char filePath[100],fileName[100];
	char dirPath[100];
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	
	cout<<"Loading g2..."<<endl;
	getPath(filePath,"public","g2");
	G2 g2 = G2FromFile(filePath);
	
	cout<<"Loading msk(g1^alpha)..."<<endl;
	getPath(filePath,label,"g1alpha");
	G1 g1alpha = G1FromFile(filePath);
	Waters_CPABEMasterKey msk(pfc,g1alpha);
	
	cout<<"Loading pubKey"<<endl;
	cout<<"Loading a"<<endl;
	getPath(filePath,label,"a");
	Big a = BigFromFile(filePath);
	G1 g1a = pfc.mult(g1,a);
	G2 g2a = pfc.mult(g2,a);
	
	cout<<"Loading alpha"<<endl;
	getPath(filePath,label,"alpha");
	Big alpha = BigFromFile(filePath);
	GT eggalpha = pfc.power(pfc.pairing(g2,g1),alpha);
	
	Waters_CPABEPublicKey pk(pfc,g1,g1a,g2,g2a,eggalpha);
	//----------------------------------
	cout<<"Generating attributes"<<endl;
/*	
	size_t attrCnt = 0;
	int d = strlen(releaseTimeStr);
	for(int i=0;i<d;i++)
	{
		attrCnt += releaseTimeStr[i] - '0';
	}
	char** attrs = new char*[attrCnt];
	char** tmp = attrs;
	unsigned int releaseTime = atoi(releaseTimeStr);
	char zeros[50] = {0};
	
	for(int i=0;i<d;i++)
	{
		int digit = releaseTime%10;
		for(int j=1;j<=digit;j++)
		{
			*tmp = new char[i+2];
			sprintf(*tmp,"%d%s",j,zeros);
			tmp++;
		}
		releaseTime /= 10;
		zeros[i] = '0';
	}
*/
	char** attrs = new char*[100];
	size_t attrCnt = getAttrsFromReleaseTime(releaseTimeStr,attrs,100);
	
	cout<<"Attrs = {";
	for(int i=0;i<attrCnt;i++)
	{
		cout<<" "<<attrs[i];
	}
	cout<<"}"<<endl;
	
	cout<<"Generating secret key"<<endl;
	Waters_CPABESecretKey sk = msk.genSecretKey(attrs,attrCnt,pk);
	
	getPath(dirPath,"public",releaseTimeStr);
	mkdir(dirPath, 0755);
	cout<<"Release key dir("<<dirPath<<") created."<<endl;
	
	ofstream out;
	
	getFileName(fileName,label,"K");
	getPath(filePath,dirPath,fileName);
	out.open(filePath);
	out << sk.K.g <<endl;
	out.close();
	
	getFileName(fileName,label,"L");
	getPath(filePath,dirPath,fileName);
	out.open(filePath);
	out << sk.L.g <<endl;
	out.close();
	
	for(int i=0;i<attrCnt;i++)
	{
		getFileName(fileName,label,attrs[i]);
		getPath(filePath,dirPath,fileName);
		out.open(filePath);
		out << sk.Kx[i].g <<endl;
		out.close();
	}
	
	return 0;
}