#include "dealerlessTRE.h"

//#define DEBUG

int loadCurve(const char* curveFile,ECn &G,Big &q,miracl* precision)
{
	char buf[500];

	ifstream common(curveFile);	/* construct file I/O streams */
	int bits,ep;
	miracl *mip=(miracl*)precision;

	ECn W;
	Big a,b,p,x,y,d;

	common >> bits;
	mip->IOBASE=16;
	common >> p >> a >> b >> q >> x >> y;
	mip->IOBASE=10;

	ecurve(a,b,p,MR_PROJECTIVE);

	//cout<<"Curve input"<<endl;
	
	if (!G.set(x,y))
	{
		//cout << "Problem - point (x,y) is not on the curve" << endl;
		return 1;
	}
	W=G;
	W*=q;
	if (!W.iszero())
	{
		//cout << "Problem - point (x,y) is not of order q" << endl;
		return 2;
	}
	
	return 0;
}
ECDSACurve loadECDSACurve(const char* curveFile,miracl* precision)
{
	ECn G;
	Big q;
	loadCurve(curveFile,G,q,precision);
	ECDSACurve curve = {G,q};
	return curve;
}
ECElgamalCurve loadECElgamalCurve(const char* curveFile,miracl* precision)
{
	ECn G;
	Big q;
	loadCurve(curveFile,G,q,precision);
	ECElgamalCurve curve = {G,q};
	return curve;
}

LSSSPolicy loadPolicy(const char* policyFile)
{
	ifstream fin(policyFile);
	LSSSPolicy policy;
	
	fin >> policy.rowCnt >> policy.colCnt;
	policy.A = (int**)calloc(policy.rowCnt,sizeof(int*));
	policy.labels = (char**)calloc(policy.rowCnt,sizeof(char*));
	
	for(int i=0;i<policy.rowCnt;i++)
	{
		policy.A[i] = (int*)calloc(policy.colCnt,sizeof(int));
		policy.labels[i] = (char*)calloc(100,sizeof(char));
		fin >> policy.labels[i];
		for(int j=0;j<policy.colCnt;j++)
		{
			fin>>policy.A[i][j];
		}
	}
	return policy;
}
void clearPolicy(LSSSPolicy policy)
{
	for(int i=0;i<policy.rowCnt;i++)
	{
		free(policy.A[i]);
		free(policy.labels[i]);
	}
	free(policy.A);
	free(policy.labels);
}

void getFileName(char* fileName,const char* name,const char* ext)
{
	strcpy(fileName,name);
	strcat(fileName,".");
	strcat(fileName,ext);
}
void getPath(char* path,const char* dirName,const char* fileName)
{
	sprintf(path,"%s/%s",dirName,fileName);
}

size_t inputFromFile(char* buf,const char* fileName)
{
	char* ptr = buf;
	FILE *in = fopen(fileName,"r");
	while(fgets(ptr,5000,in)!=NULL)
	{
		ptr += strlen(ptr);
		//cout<<buf<<endl;
	}
	fclose(in);
	return ptr-buf;
}
void outputToFile(const char* str,const char* fileName)
{
	ofstream out(fileName);
	out<<str<<endl;
}

size_t inputBinaryFromFile(BinaryData* buf,const char* fileName)
{
	FILE *in = fopen(fileName,"rb");
	
	fseek (in , 0 , SEEK_END);
	size_t lSize = ftell (in);
	rewind (in);
	
	fread(buf->data,1,lSize,in);
	buf->sz = lSize;
	fclose(in);
	
	return lSize;
}
void outputBinaryToFile(BinaryData buf,const char* fileName)
{
	FILE *out = fopen(fileName,"wb");
	
	fwrite(buf.data,1,buf.sz,out);
	fclose(out);
}

ECn ECnFromStr(char* buf)
{
	char* ptr = strtok(buf,"(,)\n");
	Big x(ptr);
	ptr = strtok(NULL,"(,)\n");
	Big y(ptr);
	ECn p(x,y);
#ifdef DEBUG
	cout<<"("<<x<<","<<y<<")"<<endl;
	cout<<p<<endl;
#endif
	return p;
}
ECn2 ECn2FromStr(char* buf)
{
	char* ptr = strtok(buf,"([,])\n");
	Big a(ptr);
	ptr = strtok(NULL,"([,])\n");
	Big b(ptr);
	ptr = strtok(NULL,"([,])\n");
	Big c(ptr);
	ptr = strtok(NULL,"([,])\n");
	Big d(ptr);
	
	ZZn2 x(a,b),y(c,d);
	ECn2 p;
	p.set(x,y);
	return p;
}
ZZn12 ZZn12FromStr(char* buf)
{
	Big num[12];
	char* ptr = strtok(buf,"([,])\n");
	num[0] = Big(ptr);
	for(int i=1;i<12;i++)
	{
		ptr = strtok(NULL,"([,])\n");
		num[i] = Big(ptr);
	}
	ZZn2 num2[6];
	for(int i=0;i<6;i++)
	{
		num2[i] = ZZn2(num[2*i],num[2*i+1]);
	}
	ZZn4 a(num2[0],num2[1]),b(num2[2],num2[3]),c(num2[4],num2[5]);
	return ZZn12(a,b,c);
}
G1 G1FromFile(const char* filePath)
{
	char buf[1000];
	size_t sz = inputFromFile(buf,filePath);
	ECn g = ECnFromStr(buf);
	G1 g1;
	g1.g = g;
	return g1;
}
G2 G2FromFile(const char* filePath)
{
	char buf[1000];
	size_t sz = inputFromFile(buf,filePath);
	ECn2 g = ECn2FromStr(buf);
	G2 g2;
	g2.g = g;
	return g2;
}
GT GTFromFile(const char* filePath)
{
	char buf[5000];
	size_t sz = inputFromFile(buf,filePath);
	ZZn12 g = ZZn12FromStr(buf);
	GT gt;
	gt.g = g;
	return gt;
}
Big BigFromFile(const char* filePath)
{
	char buf[1000];
	size_t sz = inputFromFile(buf,filePath);
	Big num(buf);
	return num;
}

LSSSPolicy policyFromReleaseTimeStr(const char* releaseTimeStr,int max)
{
	LSSSPolicy policy;
	
	int len = strlen(releaseTimeStr);
	char* rt = new char[max+1];
	memset(rt,'0',(max-len));
	memcpy(rt+(max-len),releaseTimeStr,len);
	
	policy.colCnt = max;
	policy.rowCnt = 0;
	
	bool firstnz=false;
	for(int i=0;i<max;i++)
	{
		if(rt[i] == '0')
		{
			if(!firstnz)
			{
				policy.rowCnt += 9;
			}
			policy.colCnt --;
		}
		else
		{
			firstnz = true;
			policy.rowCnt += 10 - (rt[i] -'0');
		}
	}
	
	policy.labels = new char*[policy.rowCnt];
	policy.A = new int*[policy.rowCnt];
	for(int i=0;i<policy.rowCnt;i++)
	{
		policy.labels[i] = new char[len+1];
		policy.A[i] = new int[policy.colCnt];
		memset(policy.A[i],0,policy.colCnt*sizeof(int));
	}
	
	int tp = 1;
	for(int i=1;i<max;i++)
	{
		tp *= 10;
	}
	
	int col = 0;
	int row = 0;
	firstnz=false;
	for(int i=0;i<max;i++)
	{
		if(rt[i] != '0')
			firstnz = true;
		if(!firstnz || rt[i]!='0')
		{
			for(int c='9';c>'0';c--)
			{
				sprintf(policy.labels[row],"%d",tp*(c-'0'));
				if(c == rt[i])
				{
					policy.A[row][col] = 1;
					policy.A[row][col+1] = -1;
					row++;
					col++;
					break;
				}
				else
				{
					policy.A[row][col] = 1;
					row++;
				}
			}
		}
		tp /= 10;
	}
	
	return policy;
}

void hashBig(Big x,char hash[20])
{
	sha sh;
	shs_init(&sh);
    int ch;
    while (x!=0)
    {
        ch=x % 256; x /= 256;
        shs_process(&sh,ch);
    }
	shs_hash(&sh,hash);
}

void aesEncrypt(char key[],char text[],size_t sz,char iv[])
{
	aes a;
	aes_init(&a,MR_PCFB1,16,key,iv);
	for(int j=0;j<sz;j++)
	{
		aes_encrypt(&a,&text[j]);
	}
	aes_end(&a);
}
void aesDecrypt(char key[],char text[],size_t sz,char iv[])
{
	aes a;
	aes_init(&a,MR_PCFB1,16,key,iv);
	for(int j=0;j<sz;j++)
	{
		aes_decrypt(&a,&text[j]);
	}
	aes_end(&a);
}
void aesEncryptFile(char key[],char iv[],const char inputFile[],const char outputFile[],bool encrypt)
{
	char buf[1024];
	FILE *bin=fopen(inputFile,"rb");
	FILE *bout=fopen(outputFile,"wb");
	
	fseek (bin , 0 , SEEK_END);
	size_t sz = ftell (bin);
	rewind (bin);
		
	aes a;
	aes_init(&a,MR_PCFB1,16,key,iv);
		
	while(1)
	{
		size_t bufsz = fread(buf,1,1024,bin);
		for(int i=0;i<bufsz;i++)
		{
			if(encrypt)
				aes_encrypt(&a,&buf[i]);
			else
				aes_decrypt(&a,&buf[i]);
		}
		fwrite(buf,1,bufsz,bout);
		if(sz > 1024)
		{
			sz -= 1024;
		}
		else
		{
			break;
		}
	}
	aes_end(&a);
	
	fclose(bin);
	fclose(bout);
}

size_t getAttrsFromReleaseTime(const char* releaseTimeStr,char** attrs,size_t sz)
{
	size_t attrCnt = 0;
	int d = strlen(releaseTimeStr);
	for(int i=0;i<d;i++)
	{
		attrCnt += releaseTimeStr[i] - '0';
	}
	
	if(sz<attrCnt)
	{
		attrCnt = 0;
	}
	else
	{
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
	}
	
	return attrCnt;
}