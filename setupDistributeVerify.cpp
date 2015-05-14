#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "setupDistributeVerify common policyFile sndLabel rcvLabel";
const char* ecdecrypt = "./bin/ecdecrypt";

char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

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
	
	if(argc != 5)
	{
		showUsageAndExit();
	}
	
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	const char* sndLabel=argv[3];
	const char* rcvLabel=argv[4];
	
	char buf[1500],filePath[100],_fileName[100],fileName[50],*ptr;
	size_t sz;
	
	char dkFile[50],ctFile[50];
	strcpy(_fileName,sndLabel);
	strcat(_fileName,".");
	strcat(_fileName,rcvLabel);
	getFileName(fileName,_fileName,"ct");
	getPath(ctFile,"public",fileName);
	getPath(dkFile,rcvLabel,"dk");
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	ostringstream cmd;
	cmd<<ecdecrypt<<' '<<common<<' '<<dkFile<<' '<<ctFile<<" tmp";
	cout<<"Decrypt key.."<<endl;
	cout<<cmd.str().c_str()<<endl;
	system(cmd.str().c_str());
	cout<<"Decrypt finish"<<endl;
	sz = inputFromFile(buf,"tmp");
	Big x(buf);
	cout<<"Decrypted Key = "<<x<<endl;
	char symKeyBuf[20];
	hashBig(x,symKeyBuf);
	
	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	G1 g1 = G1FromFile(filePath);
	//cout<<"g1 = "<<g1.g<<endl;
	
	cout<<"Loading g2..."<<endl;
	getPath(filePath,"public","g2");
	G2 g2 = G2FromFile(filePath);
	//cout<<"g2 = "<<g2.g<<endl;
	
	GT egg = pfc.pairing(g2,g1);
	
	cout<<"Loading encrypted a..."<<endl;
	getFileName(fileName,_fileName,"a_ct");
	getPath(filePath,"public",fileName);
	BinaryData data = {(byte*)buf,sz};
	data.sz = inputBinaryFromFile(&data,filePath);
	aesDecrypt(symKeyBuf,buf,data.sz,iv);
	cout<<"Share Initial..."<<endl;
	Share<Big> share_a;
	share_a.label = new char[strlen(rcvLabel)+1];
	strcpy(share_a.label,rcvLabel);
	Big _share_a(buf);
	share_a.share = _share_a;
	cout<<"a = "<<share_a.share<<endl;
	
	cout<<"Loading encrypted alpha..."<<endl;
	getFileName(fileName,_fileName,"alpha_ct");
	getPath(filePath,"public",fileName);
	data.data = (byte*)buf;
	data.sz = inputBinaryFromFile(&data,filePath);
	aesDecrypt(symKeyBuf,buf,data.sz,iv);
	cout<<"Share Initial..."<<endl;
	Share<Big> share_alpha;
	share_alpha.label = new char[strlen(rcvLabel)+1];
	strcpy(share_alpha.label,rcvLabel);
	Big _share_alpha(buf);
	share_alpha.share = _share_alpha;
	cout<<"alpha = "<<share_alpha.share<<endl;
	
	cout<<"Loading tokens g1a..."<<endl;
	getFileName(fileName,sndLabel,"g1a");
	getPath(filePath,"public",fileName);
	G1Group *tokens_g1_a = new G1Group[policy.colCnt];
	for(int i=0;i<policy.colCnt;i++)
	{
		ostringstream filePathBuf;
		filePathBuf << filePath << i;
		tokens_g1_a[i].g = G1FromFile(filePathBuf.str().c_str());
		cout<<tokens_g1_a[i].g.g<<endl;
	}
	
	cout<<"Loading tokens eggalpha..."<<endl;
	getFileName(fileName,sndLabel,"eggalpha");
	getPath(filePath,"public",fileName);
	GTGroup *tokens_eggalpha = new GTGroup[policy.colCnt];
	for(int i=0;i<policy.colCnt;i++)
	{
		ostringstream filePathBuf;
		filePathBuf << filePath << i;
		tokens_eggalpha[i].g = GTFromFile(filePathBuf.str().c_str());
		cout<<tokens_eggalpha[i].g.g<<endl;
	}
	
	cout<<"Verify share..."<<endl;
	DLogSetPFC(&pfc);
	VLSSS<Big,G1Group> vlsss_g1_a(policy,g1);
	if(vlsss_g1_a.verify(tokens_g1_a,policy.colCnt,share_a))
	{
		cout<<"Verify Success!!!"<<endl;
	}
	else
	{
		cout<<"Verify fail!!!"<<endl;
		return 0;
	}
	VLSSS<Big,GTGroup> vlsss_eggalpha(policy,egg);
	if(vlsss_eggalpha.verify(tokens_eggalpha,policy.colCnt,share_alpha))
	{
		cout<<"Verify Success!!!"<<endl;
	}
	else
	{
		cout<<"Verify fail!!!"<<endl;
		return 0;
	}
	
	cout<<"Save a..."<<endl;
	strcpy(fileName,sndLabel);
	strcat(fileName,".aRcv");
	getPath(filePath,rcvLabel,fileName);
	ofstream out(filePath);
	out<<share_a.share<<endl;
	
	cout<<"Save alpha..."<<endl;
	strcpy(fileName,sndLabel);
	strcat(fileName,".alphaRcv");
	getPath(filePath,rcvLabel,fileName);
	ofstream out2(filePath);
	out2<<share_alpha.share<<endl;
	
	clearPolicy(policy);
	delete share_a.label;
	delete share_alpha.label;
	
	cout<<"End"<<endl;
	
	return 0;
}