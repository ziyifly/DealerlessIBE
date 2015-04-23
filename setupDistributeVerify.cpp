#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

#include "dealerlessTRE.h"

#include <iostream>
using namespace std;

const char* usage = "setupDistributeVerify common policyFile sndLabel rcvLabel";
char iv[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

Miracl precision = 20;

void showUsageAndExit()
{
	cout<<usage<<endl;
	exit(0);
}

int main(int argc,const char** argv)
{
	if(argc != 5)
	{
		showUsageAndExit();
	}
	else
	{
		cout<<"Initial.."<<endl;
	}
	const char* common=argv[1];
	const char* policyFileName=argv[2];
	const char* sndLabel=argv[3];
	const char* rcvLabel=argv[4];
	
	char buf[500],filePath[100],_fileName[100],fileName[50],*ptr;
	size_t sz;
	
	cout<<"Loading curves..."<<endl;
	ECDSACurve ecdsaCurve = loadECDSACurve(common,&precision);
	ECElgamalCurve ecelgamalCurve = loadECElgamalCurve(common,&precision);
	
	cout<<"Loading policy..."<<endl;
	LSSSPolicy policy = loadPolicy(policyFileName);
	
	cout<<"Loading vk..."<<endl;
	getFileName(fileName,sndLabel,"vk");
	getPath(filePath,"public",fileName);
	sz = inputFromFile(buf,filePath);
	ECn qA = ECnFromStr(buf);
	ECDSAVerifyKey vk(ecdsaCurve,qA);
	
	cout<<"Loading dk..."<<endl;
	getPath(filePath,rcvLabel,"dk");
	sz = inputFromFile(buf,filePath);
	Big x(buf);
	ECElgamalDecryptKey dk(ecelgamalCurve,x);
	
	cout<<"Loading ct..."<<endl;
	strcpy(_fileName,sndLabel);
	strcat(_fileName,".");
	strcat(_fileName,rcvLabel);
	getFileName(fileName,_fileName,"ct");
	getPath(filePath,"public",fileName);
	sz = inputFromFile(buf,filePath);
	
	ECn c1 = ECnFromStr(buf);
	ECn c2 = ECnFromStr(NULL);
	ECElgamalCiphertext ct(c1,c2);
	cout<<"---"<<c1<<' '<<c2<<endl;
	
	getFileName(fileName,sndLabel,"token");
	getPath(filePath,"public",fileName);
	cout<<"Loading tokens("<<filePath<<")..."<<endl;
	sz = inputFromFile(buf,filePath);
	ECGroup *tokens = new ECGroup[policy.colCnt];
	for(int i=0;i<policy.colCnt;i++)
	{
		ECn p = ECnFromStr(i==0?buf:NULL);
		new(&tokens[i]) ECGroup(p);
		cout<<"---"<<tokens[i].p<<endl;
	}
	
	cout<<"Loading encrypted share..."<<endl;
	
	getFileName(fileName,_fileName,"share");
	getPath(filePath,"public",fileName);
	BinaryData data = {(byte*)buf,sz};
	sz = inputBinaryFromFile(&data,filePath);
	
	cout<<"Decrypting share..."<<endl;
	ECn symKey = dk.decrypt(ct);
	
	cout<<"Key = "<<symKey<<endl;
	aes a;
	sha sh;
	char symKeyBuf[20];
	
	shs_init(&sh);
	symKey.getx(x);
	hashBig(&sh,x);
	shs_hash(&sh,symKeyBuf);
	
	aes_init(&a,MR_PCFB1,16,symKeyBuf,iv);
	
	for(int j=0;j<sz;j++)
	{
		//char tmp = buf[j];
		aes_decrypt(&a,&buf[j]);
		//DEBUG..
	}
	aes_end(&a);
	buf[sz] = 0;
	cout<<sz<<" bytes shares:"<<buf<<endl;
	
	cout<<"Share Initial..."<<endl;
	Share<Big> share;
	share.label = new char[strlen(rcvLabel)+1];
	strcpy(share.label,rcvLabel);
	Big _share(buf);
	share.share = _share;
	
	//sz = share.toString(buf,500);
	//cout<<sz<<" bytes shares:"<<buf<<endl;
	
	cout<<"Verify share..."<<endl;
	VLSSS<Big,ECGroup> vlsss(policy,ecdsaCurve.g); //ABECurve
	if(vlsss.verify(tokens,policy.colCnt,share))
	{
		cout<<"Verify Success!!!"<<endl;
	}
	else
	{
		cout<<"Verify fail!!!"<<endl;
		return 0;
	}
	
	cout<<"Save share..."<<endl;
	strcpy(fileName,sndLabel);
	strcat(fileName,".shareRcv");
	getPath(filePath,rcvLabel,fileName);
	sz = share.toString(buf,500);
	//cout<<"---"<<buf<<endl;
	outputToFile(buf,filePath);
	
	clearPolicy(policy);
	delete share.label;
	
	cout<<"End"<<endl;
	
	return 0;
}