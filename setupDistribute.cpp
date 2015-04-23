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

	char buf[500],filePath[100],fileName[50];
	size_t sz;
	
	irand(time(NULL));
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	cout<<"Loading sign key..."<<endl;	
	getPath(filePath,label,"sk");
	sz = inputFromFile(buf,filePath);
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
	for(int i=0;i<shareCnt;i++)
	{
		char _fileName[100];
		strcpy(_fileName,label);
		strcat(_fileName,".");
		strcat(_fileName,shares[i].label);
		
		getFileName(fileName,shares[i].label,"ek");
		getPath(filePath,"public",fileName);
		cout<<"Loading "<<filePath<<"..."<<endl;
		sz = inputFromFile(buf,filePath);
		ECn h = ECnFromStr(buf);
		ECElgamalEncryptKey ek(ecelgamalCurve,h);
		
		Big randbig = rand(ecdsaCurve.n);
		ECn symKey = randbig*ecdsaCurve.g;
		
		ECElgamalCiphertext ct = ek.encrypt(symKey);
		
		getFileName(fileName,_fileName,"ct");
		getPath(filePath,"public",fileName);
		cout<<"Ct generate "<<filePath<<"..."<<endl;
		
		ct.toString(buf,500);
		outputToFile(buf,filePath);
		
		aes a;
		sha sh;
		Big x;
		char symKeyBuf[20];
		
		cout<<"Sym. Key = "<<symKey<<endl;
		
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
		
		getFileName(fileName,_fileName,"share");
		getPath(filePath,"public",fileName);
		
		BinaryData data = {(byte*)buf,sz};
		outputBinaryToFile(data,filePath);
		
		cout<<"Signing share ..."<<endl;
		ECDSASignature sig = sk.sign(data);
		sig.toString(buf,500);
		
		getFileName(fileName,_fileName,"shareSig");
		getPath(filePath,"public",fileName);
		
		outputToFile(buf,filePath);
		cout<<buf<<endl;
	}
	
	//----------------------------- tokens
	cout<<"Outputing tokens..."<<endl;
	char* ptr = buf;	
	for(int i=0;i<tokenCnt;i++)
	{
		ptr += tokens[i].toString(ptr,500);
		*(ptr-1) = '\n';
	}
	*(ptr-1) = 0;
	cout<<"Token="<<endl<<buf<<endl;
		
	getFileName(fileName,label,"token");
	getPath(filePath,"public",fileName);
	outputToFile(buf,filePath);
	
	//-----------------------------
	
	clearPolicy(policy);
	
	return 0;
}