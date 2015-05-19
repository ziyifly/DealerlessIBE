#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "simMPK policyFile";

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
	
	const char* policyFileName=argv[1];
	
	char filePath[200],fileName[100];
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	cout << policy <<endl;
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	//cout<<"g1 = "<<g1.g<<endl;
	
	cout<<"Loading g2..."<<endl;
	getPath(filePath,"public","g2");
	G2 g2 = G2FromFile(filePath);
	//cout<<"g2 = "<<g2.g<<endl;
	
	cout<<"Loading egg..."<<endl;
	getPath(filePath,"public","egg");
	GT egg = GTFromFile(filePath);
	
	Big a,alpha;
	/*
	Share<Big>* shares_a = new Share<Big>[policy.rowCnt];
	Share<Big>* shares_alpha = new Share<Big>[policy.rowCnt];
	LSSS<Big> lsss(policy);
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		getPath(filePath,policy.labels[i],"alpha");
		cout<<"Loading "<<filePath<<" ..."<<endl;
		shares_alpha[i].label = policy.labels[i];
		shares_alpha[i].share = BigFromFile(filePath);
	}
	lsss.reconstructSecret(shares_alpha,policy.rowCnt,&alpha);
	cout << alpha <<endl;
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		getPath(filePath,policy.labels[i],"a");
		cout<<"Loading "<<filePath<<" ..."<<endl;
		shares_a[i].label = policy.labels[i];
		shares_a[i].share = BigFromFile(filePath);
	}
	lsss.reconstructSecret(shares_a,policy.rowCnt,&a);
	cout << a <<endl;
	*/
	a = BigFromFile("sim/a");
	alpha = BigFromFile("sim/alpha");
	cout<<"------------------ Verify ----------------------"<<endl;
	
	
	cout<<"g1^a1 * g1^a2 *...*g1^an == g1^(a1+...+an)"<<endl;
	G1 leftg1,rightg1 = pfc.mult(g1,a);
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"g1a0");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		G1 g1ai = G1FromFile(filePath);
		leftg1 = leftg1 + g1ai;
	}
	cout<<"left = "<<leftg1.g<<endl;
	cout<<"right= "<<rightg1.g<<endl;
	cout<<(leftg1==rightg1?"True":"False")<<endl<<endl;
	
	cout<<"g2^a1 * g2^a2 *...*g2^an == g2^(a1+...+an)"<<endl;
	G2 leftg2,rightg2 = pfc.mult(g2,a);
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"g2a0");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		G2 g2ai = G2FromFile(filePath);
		leftg2 = leftg2 + g2ai;
	}
	cout<<"left = "<<leftg2.g<<endl;
	cout<<"right= "<<rightg2.g<<endl;
	cout<<(leftg2==rightg2?"True":"False")<<endl<<endl;
	
	cout<<"a == (a1+...+an)"<<endl;
	Big righta(0);
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"a");
		getPath(filePath,"sim",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		Big rightai = BigFromFile(filePath);
		righta = righta + rightai;
	}
	cout<<"left = "<<a<<endl;
	cout<<"right= "<<righta<<endl;
	cout<<(a==righta?"True":"False")<<endl<<endl;
	
	cout<<"alpha == (alpha1+...+alphan)"<<endl;
	Big rightalpha(0);
	for(int i=0;i<policy.rowCnt;i++)
	{
		getFileName(fileName,policy.labels[i],"alpha");
		getPath(filePath,"sim",fileName);
		cout<<"Loading "<<filePath<<" ..."<<endl;
		Big rightalphai = BigFromFile(filePath);
		rightalpha = rightalpha + rightalphai;
	}
	cout<<"left = "<<alpha<<endl;
	cout<<"right= "<<rightalpha<<endl;
	cout<<(alpha==rightalpha?"True":"False")<<endl<<endl;

	return 0;
}
