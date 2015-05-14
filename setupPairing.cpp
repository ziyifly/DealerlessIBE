#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
using namespace std;

const char* usage = "setupPairing {no parameter}";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise pairing-friendly curve
	miracl *mip=get_mip();
	mip->IOBASE = 16;
	
	if(argc != 1)
	{
		showUsageAndExit();
	}
	
	FILE *out;
	char buf[500],filePath[100],fileName[50];
	
	mkdir("public", 0755);
	cout<<"public dir created."<<endl;
	
	G1 g1;
	G2 g2;
	GT egg;
	
	pfc.random(g1);
	pfc.random(g2);
	egg = pfc.pairing(g2,g1);	
	
	cout<<"Generate g1..."<<endl;
	strcpy(fileName,"g1");
	getPath(filePath,"public",fileName);
	ofstream outG1(filePath);
	outG1 << g1.g << endl;
	
	cout<<"Generate g2..."<<endl;
	strcpy(fileName,"g2");
	getPath(filePath,"public",fileName);
	ofstream outG2(filePath);
	outG2 << g2.g << endl;
	
	cout<<"Generate egg..."<<endl;
	strcpy(fileName,"egg");
	getPath(filePath,"public",fileName);
	ofstream outGT(filePath);
	outGT << egg.g << endl;
	
	return 0;
}