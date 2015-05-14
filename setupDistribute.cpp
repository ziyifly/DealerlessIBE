#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char* usage = "setupDistribute common policyFile label";
char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

//Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	PFC pfc(AES_SECURITY);  // initialise pairing-friendly curve
	DLogSetPFC(&pfc);
	miracl *mip=get_mip();
	mip->IOBASE = 16;
	
	if(argc != 4)
	{
		showUsageAndExit();
	}
	
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	const char* label=argv[3];

	char buf[10000],filePath[100],fileName[50];
	size_t sz;
	
	irand(time(NULL));

	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);

	cout<<"Loading g1..."<<endl;
	getPath(filePath,"public","g1");
	sz = inputFromFile(buf,filePath);
	G1 g1;
	g1.g = ECnFromStr(buf);
	cout<<"g1("<<sz<<" bytes) = "<<g1.g<<endl;

	cout<<"Loading g2..."<<endl;
	getPath(filePath,"public","g2");
	sz = inputFromFile(buf,filePath);
	G2 g2;
	g2.g = ECn2FromStr(buf);
	cout<<"g2("<<sz<<" bytes) = "<<g2.g<<endl;
	
	GT egg = pfc.pairing(g2,g1);

	//VLSSS<Big,ECGroup> vlsss(policy,ecdsaCurve.g); //ABECurve
	VLSSS<Big,G1Group> vlsss_g1_a(policy,g1);
	VLSSS<Big,G2Group> vlsss_g2_a(policy,g2);
	VLSSS<Big,GTGroup> vlsss_egg_alpha(policy,egg);
	
	//Big *r = new Big[policy.colCnt];
	Big *r_a = new Big[policy.colCnt];
	Big *r_alpha = new Big[policy.colCnt];

	for(int i=0;i<policy.colCnt;i++)
	{
		//r[i] = rand(ecdsaCurve.n);
		pfc.random(r_a[i]);
		pfc.random(r_alpha[i]);
	}
	
	//vlsss.setR(r);
	vlsss_g1_a.setR(r_a);
	vlsss_g2_a.setR(r_a);
	vlsss_egg_alpha.setR(r_alpha);
	
	//Share<Big> *shares;
	Share<Big> *shares_a,*shares_alpha;
	//ECGroup *tokens;
	G1Group *tokens_g1_a;
	G2Group *tokens_g2_a;
	GTGroup *tokens_egg_alpha;	
	size_t shareCnt,tokenCnt;

	cout<<"Generating shares..."<<endl;
	//vlsss.genShares(&shares,&shareCnt);
	vlsss_g1_a.genShares(&shares_a,&shareCnt);
	vlsss_egg_alpha.genShares(&shares_alpha,&shareCnt);
	cout<<"Generating tokens..."<<endl;	
	//vlsss.genVerifiableTokens(&tokens,&tokenCnt);
	vlsss_g1_a.genVerifiableTokens(&tokens_g1_a,&tokenCnt);
	vlsss_g2_a.genVerifiableTokens(&tokens_g2_a,&tokenCnt);
	vlsss_egg_alpha.genVerifiableTokens(&tokens_egg_alpha,&tokenCnt);

	//----------------------------- tokens
	cout<<"Outputing tokens..."<<endl;
	ostringstream outbuf;
	
	cout<<"Token:g1^a="<<endl;
	getFileName(fileName,label,"g1a");
	getPath(filePath,"public",fileName);
	for(int i=0;i<tokenCnt;i++)
	{
		outbuf.clear();
		outbuf.str("");
		outbuf << filePath << i;
		ofstream output(outbuf.str().c_str());
		output << tokens_g1_a[i].g.g << endl;
		cout << tokens_g1_a[i].g.g << endl;
	}
	
	cout<<"Token:g2^a="<<endl;
	getFileName(fileName,label,"g2a");
	getPath(filePath,"public",fileName);
	for(int i=0;i<tokenCnt;i++)
	{
		outbuf.clear();
		outbuf.str("");
		outbuf << filePath << i;
		ofstream output(outbuf.str().c_str());
		output << tokens_g2_a[i].g.g << endl;
		cout << tokens_g2_a[i].g.g << endl;
	}
	
	cout<<"Token:egg^alpha="<<endl;
	getFileName(fileName,label,"eggalpha");
	getPath(filePath,"public",fileName);
	for(int i=0;i<tokenCnt;i++)
	{
		outbuf.clear();
		outbuf.str("");
		outbuf << filePath << i;
		ofstream output(outbuf.str().c_str());
		output << tokens_egg_alpha[i].g.g << endl;
		cout << tokens_egg_alpha[i].g.g << endl;
	}
	
	//----------------------------- shares
	cout<<"Loading curves..."<<endl;
	mip=get_mip();
	ECDSACurve ecdsaCurve = loadECDSACurve(common,mip);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,mip);
	mip->IOBASE = 16;
	
	cout<<"Loading sign key..."<<endl;
	getPath(filePath,label,"sk");
	sz = inputFromFile(buf,filePath);
	Big dA(buf);
	ECDSASignKey sk(ecdsaCurve,dA);

	cout<<"Encrypting shares..."<<endl;
	for(int i=0;i<shareCnt;i++)
	{
		char _fileName[100];
		strcpy(_fileName,label);
		strcat(_fileName,".");
		strcat(_fileName,shares_a[i].label);
		
		getFileName(fileName,shares_a[i].label,"ek");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<"..."<<endl;
		sz = inputFromFile(buf,filePath);
		ECn h = ECnFromStr(buf);
		ECElgamalEncryptKey ek(ecelgamalCurve,h);
		
		Big randbig = rand(ecdsaCurve.n);
		ECn symKey = randbig*ecdsaCurve.g;
		cout<<"\tSymmetric Key = "<<symKey<<endl;
		
		ECElgamalCiphertext ct = ek.encrypt(symKey);
		getFileName(fileName,_fileName,"ct");
		getPath(filePath,"public",fileName);
		cout<<"\tCt generate "<<filePath<<"..."<<endl;
		ct.toString(buf,500);
		outputToFile(buf,filePath);
		
		Big x;
		symKey.getx(x);
		char symKeyBuf[20];
		hashBig(x,symKeyBuf);
		//--------------------------------------------
		int sz = shares_a[i].toString(buf,500);
		cout<<sz<<" bytes a:"<<buf<<endl;
		
		getFileName(fileName,_fileName,"a_ct");
		getPath(filePath,"public",fileName);
		aesEncrypt(symKeyBuf,buf,sz,iv);
		BinaryData data = {(byte*)buf,sz};
		outputBinaryToFile(data,filePath);
		
		cout<<"Signing a ..."<<endl;
		getFileName(fileName,_fileName,"a_sig");
		getPath(filePath,"public",fileName);
		ECDSASignature sig = sk.sign(data);
		sig.toString(buf,500);
		outputToFile(buf,filePath);
		cout<<buf<<endl;
		//----------------------------------------
		sz = shares_alpha[i].toString(buf,500);
		cout<<sz<<" bytes alpha:"<<buf<<endl;
		
		getFileName(fileName,_fileName,"alpha_ct");
		getPath(filePath,"public",fileName);
		aesEncrypt(symKeyBuf,buf,sz,iv);
		data.sz = sz;
		outputBinaryToFile(data,filePath);
		
		cout<<"Signing alpha ..."<<endl;
		getFileName(fileName,_fileName,"alpha_sig");
		getPath(filePath,"public",fileName);
		sig = sk.sign(data);
		sig.toString(buf,500);		
		outputToFile(buf,filePath);
		cout<<buf<<endl;
	}
	//-----------------------------
	
	getFileName(fileName,label,"a");
	getPath(filePath,"sim",fileName);
	ofstream simouta(filePath);
	simouta << r_a[0] <<endl;
	
	getFileName(fileName,label,"alpha");
	getPath(filePath,"sim",fileName);
	ofstream simoutalpha(filePath);
	simoutalpha << r_alpha[0] <<endl;
	
	clearPolicy(policy);
	
	return 0;
}