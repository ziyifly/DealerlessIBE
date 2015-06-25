#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "userkeygen user";

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise PFC
	miracl* mip=get_mip();
	
	if(argc != 2)
	{
		showUsageAndExit();
	}
	
	const char* userStr = argv[1];
	
	mkdir(userStr, 0755);
	cout<<userStr<<" dir created."<<endl;
	
	char filePath[100],fileName[100];
	char dirPath[100];
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	
	Big sk;
	pfc.random(sk);
	
	G1 pk = pfc.mult(g1,sk);
	
	ofstream out;
	
	getPath(filePath,userStr,"pk");
	out.open(filePath);
	out<<pk.g<<endl;
	out.close();
	
	getPath(filePath,userStr,"sk");
	out.open(filePath);
	out<<sk<<endl;
	out.close();
	
	return 0;
}