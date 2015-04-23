#include "dealerlessTRE.h"
#include <fstream>

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
	while(fgets(ptr,500,in)!=NULL)
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
	return p;
}

void hashBig(sha *s,Big x)
{
    int ch;
    while (x!=0)
    {
        ch=x % 256; x /= 256;
        shs_process(s,ch);
    }
}