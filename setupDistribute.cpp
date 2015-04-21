#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupDistribute common policyFile label";
char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	if(argc != 4)
	{
		showUsageAndExit();
	}
	else
	{
		cout<<"Initial.."<<endl;
	}
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	const char* label=argv[3];

	char buf[500],fileName[50];
	size_t sz;
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	cout<<"Loading sign key..."<<endl;	
	getPath(fileName,label,"sk");
	sz = inputFromFile(buf,fileName);
	Big dA(buf);
	ECDSASignKey sk(ecdsaCurve,dA);
	
	VLSSS<Big,ECGroup> vlsss(policy,ecdsaCurve.g); //ABECurve
	
	Big *r = new Big[policy.colCnt];
	for(int i=0;i<policy.colCnt;i++)
	{
		r[i] = rand(ecdsaCurve.n);
	}
	
	vlsss.setR(r);
	Share<Big> *shares;
	ECGroup *tokens;
	size_t shareCnt,tokenCnt;
	
	cout<<"Generating shares..."<<endl;	
	vlsss.genShares(&shares,&shareCnt);
	cout<<"Generating tokens..."<<endl;	
	vlsss.genVerifiableTokens(&tokens,&tokenCnt);
	
	//----------------------------- shares
	cout<<"Encrypting shares..."<<endl;
	//irand(time(NULL));
	for(int i=0;i<shareCnt;i++)
	{
		getPath(fileName,shares[i].label,"ek");
		sz = inputFromFile(buf,fileName);
		cout<<fileName<<" read."<<endl;
		ECn h = ECnFromStr(buf);
		ECElgamalEncryptKey ek(ecelgamalCurve,h);
		
		Big randbig = rand(ecdsaCurve.n);
		ECn symKey = randbig*ecdsaCurve.g;
		
		ECElgamalCiphertext ct = ek.encrypt(symKey);
		
		ct.toString(buf,500);
		char shareKey[50];
		strcpy(shareKey,shares[i].label);
		strcat(shareKey,".ct");
		getPath(fileName,label,shareKey);
		outputToFile(buf,fileName);
		
		aes a;
		sha sh;
		Big x;
		char symKeyBuf[20];
		
		cout<<"Key = "<<symKey<<endl;
		
		shs_init(&sh);
		symKey.getx(x);
		hashBig(&sh,x);
		shs_hash(&sh,symKeyBuf);
		
		aes_init(&a,MR_PCFB1,16,symKeyBuf,iv);
		int sz = shares[i].toString(buf,500);
		cout<<sz<<" bytes shares:"<<buf<<endl;
		
		for(int j=0;j<sz;j++)
		{
			//char tmp = buf[j];
			aes_encrypt(&a,&buf[j]);
			//DEBUG..
		}
		aes_end(&a);
		buf[sz] = 0;
		
		char shareName[50];
		strcpy(shareName,shares[i].label);
		strcat(shareName,".share");
		getPath(fileName,label,shareName);
		outputToFile(buf,fileName);
		
		//------------------ sign
	}
	
	//----------------------------- tokens
	cout<<"Outputing tokens..."<<endl;
	char* ptr = buf;	
	for(int i=0;i<tokenCnt;i++)
	{
		ptr += tokens[i].toString(ptr,500);
		*(ptr-1) = '\n';
	}
	cout<<"Token="<<endl<<buf<<endl;
		
	getPath(fileName,label,"token");
	outputToFile(buf,fileName);
	
	//-----------------------------
	
	clearPolicy(policy);
	
	return 0;
}